#include "video_encoder.hpp"
#include <boost/stacktrace.hpp>
#include <common_video/h264/h264_bitstream_parser.h>
#include <common_video/h264/h264_common.h>
#include <fmt/format.h>
#include <fubble/rtc/logger.hpp>
#include <fubble/utils/exception.hpp>
#include <modules/video_coding/include/video_codec_interface.h>
#include <modules/video_coding/include/video_error_codes.h>
#include <system_wrappers/include/clock.h>
#include <thread>
#include <unordered_set>
extern "C" {
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>  // mmap
#include <sys/prctl.h> // thread naming
#include <sys/stat.h>
#include <sys/types.h>
}

using namespace fubble::v4l2_hw_h264;

struct v4l2_error : utils::exception {};
using error_description =
    boost::error_info<struct error_description_tag, std::string>;
using errno_info = boost::error_info<struct errno_tag, int>;
using errno_str_info = boost::error_info<struct errno_str_tag, std::string>;
using device_info = boost::error_info<struct device_tag, std::string>;

namespace {
// mostly copied from v4l2 capture example
// https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/capture.c.html
#define CLEAR(x) memset(&(x), 0, sizeof(x))
struct v4l2_h264_reader {
  struct buffer {
    void *start{};
    size_t length{};
  };
  using buffers_type = std::vector<buffer>;

  rtc::logger logger{"v4l2_h264_reader"};
  const config config_;
  using data_callback = std::function<void(const void *data, int size)>;
  data_callback on_data;
  const std::string &device = config_.path;
  const char *device_c = device.c_str();
  int fd{-1};
  buffers_type buffers;
  std::thread read_thread;
  bool run{false};
  std::unordered_set<int> available_ctrls;

  v4l2_h264_reader(const config &config_, data_callback callback)
      : config_{config_}, on_data{callback} {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  ~v4l2_h264_reader() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  void enumerate_ctrls() {
    // https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/control.html
    v4l2_queryctrl queryctrl;
    CLEAR(queryctrl);
    queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;

      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << ", control.name: "
          << reinterpret_cast<const char *>(queryctrl.name);
      available_ctrls.insert(queryctrl.id);
      queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    if (errno != EINVAL)
      throw_errno("VIDIOC_QUERYCTRL");
  }

  void throw_description(const std::string &description) {
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "throwing, device: '{}', description: '{}'", device, description);
    BOOST_THROW_EXCEPTION(v4l2_error() << error_description(description)
                                       << device_info(device));
  }

  void throw_errno(const std::string &description) {
    std::string errno_str = strerror(errno);
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "throwing, device: '{}', errno: {}, strerror: '{}', description: '{}'",
        device, errno, errno_str, description);
    BOOST_THROW_EXCEPTION(
        v4l2_error() << errno_info(errno) << errno_str_info(strerror(errno))
                     << error_description(description) << device_info(device));
  }

  int xioctl_throw(int fd, unsigned long int request, void *arg,
                   std::string description) {
    auto result = xioctl(fd, request, arg);
    if (result < 0)
      throw_errno(description);
    return result;
  }

  int xioctl(int fh, unsigned long int request, void *arg) {
    int result{};

    do {
      result = ioctl(fh, request, arg);
    } while (-1 == result && EINTR == errno);

    return result;
  }
  void open_device() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    struct stat st;
    if (-1 == stat(device_c, &st))
      throw_errno("Cannot identify device");
    if (!S_ISCHR(st.st_mode))
      throw_description("not a linux-device");
    fd = open(device_c, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == fd)
      throw_description("can't open device");
  }

  void init_mmap() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
      if (EINVAL == errno)
        throw_errno("does not support memory mapping");
      throw_errno("VIDIOC_REQBUFS");
    }

    if (req.count < 2)
      throw_description("Insufficient buffer memory");

    buffers.resize(req.count);

    for (std::size_t index{}; index < req.count; ++index) {
      buffer &buffer_ = buffers[index];

      struct v4l2_buffer buf;
      CLEAR(buf);
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index = index;
      xioctl_throw(fd, VIDIOC_QUERYBUF, &buf, "VIDIOC_QUERYBUF");

      buffer_.length = buf.length;
      buffer_.start = mmap(NULL /* start anywhere */, buf.length,
                           PROT_READ | PROT_WRITE /* required */,
                           MAP_SHARED /* recommended */, fd, buf.m.offset);

      if (MAP_FAILED == buffer_.start)
        throw_errno("mmap");
    }
  }

  void init_device() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    unsigned int min;

    struct v4l2_capability cap;
    if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
      if (EINVAL == errno)
        throw_errno("It's no V4L2 device");
      else
        throw_errno("VIDIOC_QUERYCAP");
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
      throw_description("device is no video capture device. "
                        "cap.capabilities & V4L2_CAP_VIDEO_CAPTURE");

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
      throw_description("device does not support streaming i/o");

    /* Select video input, video standard and tune here. */
    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
      crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      crop.c = cropcap.defrect; /* reset to default */

      if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
        switch (errno) {
        case EINVAL:
          /* Cropping not supported. */
          break;
        default:
          /* Errors ignored. */
          break;
        }
      }
    } else {
      /* Errors ignored. */
    }

    struct v4l2_format fmt;
    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    const bool set_format = true;
    if (set_format) {
      fmt.fmt.pix.width = config_.width;
      fmt.fmt.pix.height = config_.height;
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
      fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

      // Note VIDIOC_S_FMT may change width and height.
      xioctl_throw(fd, VIDIOC_S_FMT, &fmt, "VIDIOC_S_FMT");

      struct v4l2_streamparm parm;
      CLEAR(parm);
      parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      parm.parm.capture.timeperframe.numerator = 1;
      parm.parm.capture.timeperframe.denominator = config_.framerate;
      xioctl_throw(fd, VIDIOC_S_PARM, &parm, "VIDIOC_S_PARM");
      // inside parm gets the actual set framerate set. log?

      // v4l2-ctl --set-ctrl repeat_sequence_header=1
      // this is a must, if available! else webrtc won't be able to handle the
      // h264 frame (SPS/PPS errors)
      if (available_ctrls.find(V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER) !=
          available_ctrls.cend()) {
        v4l2_control seq_header;
        CLEAR(seq_header);
        seq_header.id = V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER;
        seq_header.value = 1;
        xioctl_throw(fd, VIDIOC_S_CTRL, &seq_header,
                     "VIDIOC_S_CTRL, V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER");
      }
    } else {
      // Preserve original settings as set by v4l2-ctl for example
      xioctl_throw(fd, VIDIOC_G_FMT, &fmt, "VIDIOC_G_FMT");
    }

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
      fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
      fmt.fmt.pix.sizeimage = min;

    init_mmap();
  }

  void trigger_i_frame() {
    // TODO does not work
    // https://github.com/raspberrypi/linux/issues/3171#issuecomment-809332554
    // creates error "invalid argument"
    return;
#ifdef V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME
    v4l2_control control;
    CLEAR(control);
    control.id = V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME;
    xioctl_throw(fd, VIDIOC_S_CTRL, &control,
                 "VIDIOC_S_CTRL, V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME");
#endif
  }

  void start_capturing() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;

    for (std::size_t index = 0; index < buffers.size(); ++index) {
      struct v4l2_buffer buf;

      CLEAR(buf);
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index = index;

      xioctl_throw(fd, VIDIOC_QBUF, &buf, "VIDIOC_QBUF");
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl_throw(fd, VIDIOC_STREAMON, &type, "VIDIOC_STREAMON");
  }

  void mainloop() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
#if 1
    if (run) {
#if 0
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", already running";
#endif
      return;
    }
    run = true;
#endif
    read_thread = std::thread([this] {
      prctl(PR_SET_NAME, "rtc_v4l2_read", 0, 0, 0);
      while (run) {
        fd_set fds;
        struct timeval tv;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int result = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == result) {
          if (EINTR == errno)
            continue;
          throw_errno("select");
        }
        if (0 == result) {
          BOOST_LOG_SEV(logger, logging::severity::error)
              << __FUNCTION__ << "select timeout";
          continue;
        }

        read_frame();
      }
    });
  }

  int read_frame() {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
#endif
    struct v4l2_buffer buf;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
      switch (errno) {
      case EAGAIN:
        return 0;

      case EIO:
        /* Could ignore EIO, see spec. */
        /* fall through */
      default:
        throw_errno("VIDIOC_DQBUF");
      }
    }
    BOOST_ASSERT(buf.index < buffers.size());
    process_image(buffers[buf.index].start, buf.bytesused);
    xioctl_throw(fd, VIDIOC_QBUF, &buf, "VIDIOC_QBUF");

    return 1;
  }

  void process_image(const void *p, int size) {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", size: " << size;
#endif
    if (!run)
      return;
    on_data(p, size);
  }

  void stop_capturing(void) {
    run = false;
    if (read_thread.joinable())
      read_thread.join();

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl_throw(fd, VIDIOC_STREAMOFF, &type, "VIDIOC_STREAMOFF");
  }

  void uninit_device() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    for (auto &buffer_ : buffers)
      if (-1 == munmap(buffer_.start, buffer_.length))
        throw_errno("munmap");
    buffers.clear();
  }

  void close_device() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    if (-1 == close(fd))
      throw_errno("close");
    fd = -1;
  }

  void set_bitrate(int bitrate) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", bitrate: " << bitrate;

    if (available_ctrls.find(V4L2_CID_MPEG_VIDEO_BITRATE) ==
        available_ctrls.cend())
      return;

    if (available_ctrls.find(V4L2_CID_MPEG_VIDEO_BITRATE_MODE) !=
        available_ctrls.cend()) {
      struct v4l2_control control;
      CLEAR(control);
      control.id = V4L2_CID_MPEG_VIDEO_BITRATE_MODE;
      // V4L2_MPEG_VIDEO_BITRATE_MODE_VBR // variable
      control.value =
          V4L2_MPEG_VIDEO_BITRATE_MODE_CBR; // cbr -> constant bitrate
      xioctl_throw(fd, VIDIOC_S_CTRL, &control,
                   "VIDIOC_S_CTRL, V4L2_CID_MPEG_VIDEO_BITRATE_MODE");
    }

    struct v4l2_control control;
    CLEAR(control);
    control.id = V4L2_CID_MPEG_VIDEO_BITRATE;
    control.value = bitrate;
    xioctl_throw(fd, VIDIOC_S_CTRL, &control,
                 "VIDIOC_S_CTRL, V4L2_CID_MPEG_VIDEO_BITRATE");
  }
};

class video_encoder_impl : public video_encoder {
public:
  webrtc::EncodedImageCallback *callback{};
  webrtc::Clock *const clock_{webrtc::Clock::GetRealTimeClock()};
  const config config_;
  std::unique_ptr<v4l2_h264_reader> reader;
  bool got_first_iframe{};

  video_encoder_impl(const config &config_) : config_{config_} {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  ~video_encoder_impl() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  void SetFecControllerOverride(
      webrtc::FecControllerOverride *fec_controller_override) override {
    (void)fec_controller_override;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  }

  int InitEncode(const webrtc::VideoCodec *codec_settings,
                 const VideoEncoder::Settings &settings) override {
    (void)settings;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", codec_settings->width: " << codec_settings->width;
    BOOST_ASSERT(!reader);
    reader = std::make_unique<v4l2_h264_reader>(
        config_, [this](auto data, auto size) { on_data(data, size); });
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", open_device";
    reader->open_device();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", enumerate_ctrls";
    reader->enumerate_ctrls();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", init_device";
    reader->init_device();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", set_bitrate";
    reader->set_bitrate(codec_settings->startBitrate);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", start_capturing";
    reader->start_capturing();
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", mainloop";
    reader->mainloop();
#endif
    return WEBRTC_VIDEO_CODEC_OK;
  }

  bool is_h264_i_frame(const std::uint8_t *packet) {
    // https://stackoverflow.com/questions/1957427/detect-mpeg4-h264-i-frame-idr-in-rtp-stream
    int RTPHeaderBytes = 4;

    int fragment_type = packet[RTPHeaderBytes + 0] & 0x1F;
    int nal_type = packet[RTPHeaderBytes + 1] & 0x1F;
    int start_bit = packet[RTPHeaderBytes + 1] & 0x80;

    bool result{};
    if (((fragment_type == 28 || fragment_type == 29) && nal_type == 5 &&
         start_bit == 128) ||
        fragment_type == 5) {
      result = true;
    }

#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << std::hex << ", 0x" << int(packet[0]) << ", 0x"
        << int(packet[1]) << ", 0x" << int(packet[2]) << ", 0x"
        << int(packet[3]) << ", 0x" << int(packet[4]) << ", 0x"
        << int(packet[5]) << ", 0x" << int(packet[6]) << ", 0x"
        << int(packet[7]) << ", 0x" << int(packet[8]) << ", 0x"
        << int(packet[9]) << std::dec << ", result: " << result;
#endif

    // hack! the above code didn't get the iframe. check what the next line is
    // actually doiing!
    // checkout https://www.cardinalpeak.com/the-h-264-sequence-parameter-set
    // and https://en.wikipedia.org/wiki/Network_Abstraction_Layer
    if (int(packet[4]) == 0x27) {
#if 0
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << " hack";
#endif
      return true;
    }

    return result;
  }

  void on_data(const void *data, int size) {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << __FUNCTION__ << ", size:" << size;
#endif
    // TODO here we r using a deprecated method
    webrtc::EncodedImage encoded{
        static_cast<std::uint8_t *>(const_cast<void *>(data)),
        static_cast<std::size_t>(size), static_cast<std::size_t>(size)};
    webrtc::CodecSpecificInfo info;
    webrtc::CodecSpecificInfoH264 codec_info;
    codec_info.packetization_mode =
        webrtc::H264PacketizationMode::NonInterleaved;
    codec_info.temporal_idx = webrtc::kNoTemporalIdx;
    auto frameType = webrtc::VideoFrameType::kVideoFrameDelta;
    auto is_intra_frame = is_h264_i_frame(static_cast<const uint8_t *>(data));
    if (is_intra_frame) {
      if (!got_first_iframe) {
        BOOST_LOG_SEV(logger, logging::severity::debug)
            << __FUNCTION__ << ", got_first_iframe = true";
        got_first_iframe = true;
      }
#if 0
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << ", idr_frame detected";
#endif
      frameType = webrtc::VideoFrameType::kVideoFrameKey;
    }
#if 0 // TODO neccesairy on rpi?
    if (!got_first_iframe) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << " !got_first_iframe";
      return;
    }
#endif
    codec_info.idr_frame = encoded._frameType == frameType;
    codec_info.base_layer_sync = false;

    h264_bitstream_parser_.ParseBitstream(rtc::ArrayView<const std::uint8_t>(
        static_cast<const std::uint8_t *>(data), size));
    encoded.qp_ = h264_bitstream_parser_.GetLastSliceQp().value_or(-1);

    const std::vector<webrtc::H264::NaluIndex> nalu_indexes =
        webrtc::H264::FindNaluIndices(static_cast<const std::uint8_t *>(data),
                                      size);

    if (nalu_indexes.empty()) {
      // could not find the nal unit in the buffer, so do nothing.
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << "NAL unit length is zero! "
          << "Frame length : " << size;
      return;
    };

    int64_t capture_time_ms = clock_->TimeInMilliseconds();
    // int64_t ntp_capture_time_ms = clock_->CurrentNtpInMilliseconds();

    encoded.SetTimestamp(capture_time_ms);
    encoded.SetSpatialIndex(0);
    encoded._frameType = frameType;
    // encoded.ntp_time_ms_ = ntp_capture_time_ms;
    // encoded.capture_time_ms_ = capture_time_ms;
    // encoded._encodedWidth = config_.width;
    // encoded._encodedHeight = config_.height;

    info.codecSpecific.H264 = codec_info;
    info.codecType = webrtc::kVideoCodecH264;
    // info.end_of_picture = true;
    if (!callback) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << ", callback not set";
      return;
    }
    webrtc::EncodedImageCallback::Result result =
        callback->OnEncodedImage(encoded, &info);
    if (result.error ==
        webrtc::EncodedImageCallback::Result::ERROR_SEND_FAILED) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__
          << ", error in passing EncodedImage, ERROR_SEND_FAILED";
      // consequences?
    }
  }

  int32_t RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback *callback_) override {
    callback = callback_;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int32_t Release() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    reader->stop_capturing();
    reader->uninit_device();
    reader->close_device();
    reader.reset();
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int32_t
  Encode(const webrtc::VideoFrame &frame,
         const std::vector<webrtc::VideoFrameType> *frame_types) override {
    // this function gets called with width=1 if the remote client demands an
    // iframe
    (void)frame;
    (void)frame_types;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", frame.width: " << frame.width();
    reader->mainloop();
    if (frame_types != nullptr) {
      // We only support a single stream.
      RTC_DCHECK_EQ(frame_types->size(), static_cast<size_t>(1));
      // Skip frame?
      if ((*frame_types)[0] == webrtc::VideoFrameType::kEmptyFrame) {
        return WEBRTC_VIDEO_CODEC_OK;
      }
      // Force key frame?
      if ((*frame_types)[0] == webrtc::VideoFrameType::kVideoFrameKey)
        reader->trigger_i_frame();
    }
    return WEBRTC_VIDEO_CODEC_OK;
  }

  void SetRates(const RateControlParameters &parameters) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", framerate_fps: " << parameters.framerate_fps
        << ", bandwidth_allocation: " << parameters.bandwidth_allocation.kbps()
        << ", bitrate: " << parameters.bitrate.ToString()
        << ", target_bitrate: " << parameters.target_bitrate.ToString();
    reader->set_bitrate(parameters.target_bitrate.get_sum_bps());
  }

  EncoderInfo GetEncoderInfo() const override {
    auto info = webrtc::VideoEncoder::GetEncoderInfo();
    // webrtc::VideoEncoder::EncoderInfo info;
    info.has_internal_source = true;
    info.is_hardware_accelerated = true;
    info.implementation_name = "v4l2_hw_h264";
    info.scaling_settings = webrtc::VideoEncoder::ScalingSettings::kOff;
    // TODO don't use fixed values
    info.resolution_bitrate_limits = {VideoEncoder::ResolutionBitrateLimits(
        1280 * 720, 25'000, 25'000, 25'000'000)};
    // info.supports_simulcast = false; // not needed. `false` is default
    BOOST_LOG_SEV(const_cast<video_encoder_impl *>(this)->logger,
                  logging::severity::debug)
        << __FUNCTION__ << ", info: " << info.ToString();
    return info;
  }

private:
  rtc::logger logger{"video_encoder_impl"};
  webrtc::H264BitstreamParser h264_bitstream_parser_;
};
} // namespace

std::unique_ptr<video_encoder> video_encoder::create(const config &config_) {
  return std::make_unique<video_encoder_impl>(config_);
}
