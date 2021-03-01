#include "video_encoder.hpp"
#include <boost/stacktrace.hpp>
#include <common_video/h264/h264_bitstream_parser.h>
#include <common_video/h264/h264_common.h>
#include <fubble/rtc/logger.hpp>
#include <modules/video_coding/include/video_codec_interface.h>
#include <modules/video_coding/include/video_error_codes.h>
#include <system_wrappers/include/clock.h>
#include <thread>
extern "C" {
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h> // mmap
#include <sys/stat.h>
#include <sys/types.h>
}

using namespace fubble::v4l2_hw_h264;

namespace {
#define CLEAR(x) memset(&(x), 0, sizeof(x))
struct v4l2_h264_reader {
  struct buffer {
    void *start;
    size_t length;
  };
  enum io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
  };

  rtc::logger logger{"v4l2_h264_reader"};
  const config config_;
  using data_callback = std::function<void(const void *data, int size)>;
  data_callback on_data;
  const std::string &device = config_.path;
  const char *device_c = device.c_str();
  int fd{-1};
  struct buffer *buffers{};
  // TODO refactor the usage of n_buffers
  unsigned int n_buffers{};
  io_method io = IO_METHOD_MMAP;
  std::thread read_thread;
  bool run{true};

  v4l2_h264_reader(const config &config_, data_callback callback)
      : config_{config_}, on_data{callback} {}

  int xioctl(int fh, unsigned long int request, void *arg) {
    int result;

    do {
      result = ioctl(fh, request, arg);
    } while (-1 == result && EINTR == errno);

    return result;
  }
  void errno_exit(const char *s) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", errno: " << errno << ", description: " << s
        << ", stacktrace:\n"
        << boost::stacktrace::stacktrace();
    std::exit(1);
  }
  void open_device() {
    struct stat st;
    if (-1 == stat(device_c, &st)) {
      fprintf(stderr, "Cannot identify '%s': %d, %s\\n", device_c, errno,
              strerror(errno));
      std::exit(1);
    }
    if (!S_ISCHR(st.st_mode)) {
      fprintf(stderr, "%s is no devicen", device_c);
      std::exit(1);
    }
    fd = open(device_c, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == fd) {
      fprintf(stderr, "Cannot open '%s': %d, %s\\n", device_c, errno,
              strerror(errno));
      std::exit(1);
    }
  }

  void init_mmap() {
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
      if (EINVAL == errno) {
        fprintf(stderr,
                "%s does not support "
                "memory mappingn",
                device_c);
        exit(EXIT_FAILURE);
      } else {
        errno_exit("VIDIOC_REQBUFS");
      }
    }

    if (req.count < 2) {
      fprintf(stderr, "Insufficient buffer memory on %s\\n", device_c);
      exit(EXIT_FAILURE);
    }

    buffers = static_cast<buffer *>(calloc(req.count, sizeof(*buffers)));

    if (!buffers) {
      fprintf(stderr, "Out of memory\\n");
      exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
      struct v4l2_buffer buf;

      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index = n_buffers;

      if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
        errno_exit("VIDIOC_QUERYBUF");

      buffers[n_buffers].length = buf.length;
      buffers[n_buffers].start =
          mmap(NULL /* start anywhere */, buf.length,
               PROT_READ | PROT_WRITE /* required */,
               MAP_SHARED /* recommended */, fd, buf.m.offset);

      if (MAP_FAILED == buffers[n_buffers].start)
        errno_exit("mmap");
    }
  }

  void init_device() {
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
      if (EINVAL == errno) {
        fprintf(stderr, "%s is no V4L2 device\\n", device_c);
        exit(EXIT_FAILURE);
      } else {
        errno_exit("VIDIOC_QUERYCAP");
      }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
      fprintf(stderr, "%s is no video capture device\\n", device_c);
      exit(EXIT_FAILURE);
    }

    switch (io) {
    case IO_METHOD_READ:
      if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        fprintf(stderr, "%s does not support read i/o\\n", device_c);
        exit(EXIT_FAILURE);
      }
      break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming i/o\\n", device_c);
        exit(EXIT_FAILURE);
      }
      break;
    }

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

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bool force_format = true;
    if (force_format) {
      fmt.fmt.pix.width = config_.width;
      fmt.fmt.pix.height = config_.height;
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
      fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

      if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        errno_exit("VIDIOC_S_FMT");

      /* Note VIDIOC_S_FMT may change width and height. */
    } else {
      /* Preserve original settings as set by v4l2-ctl for example */
      if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
        errno_exit("VIDIOC_G_FMT");
    }

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
      fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
      fmt.fmt.pix.sizeimage = min;

    switch (io) {
    case IO_METHOD_READ:
      init_read(fmt.fmt.pix.sizeimage);
      break;

    case IO_METHOD_MMAP:
      init_mmap();
      break;

    case IO_METHOD_USERPTR:
      BOOST_ASSERT(false);
      // init_userp(fmt.fmt.pix.sizeimage);
      break;
    }
  }
  void init_read(unsigned int buffer_size) {
    buffers = static_cast<buffer *>(calloc(1, sizeof(*buffers)));

    if (!buffers) {
      fprintf(stderr, "Out of memory\\n");
      exit(EXIT_FAILURE);
    }

    buffers[0].length = buffer_size;
    buffers[0].start = malloc(buffer_size);

    if (!buffers[0].start) {
      fprintf(stderr, "Out of memory\\n");
      exit(EXIT_FAILURE);
    }
  }
  void start_capturing() {
    unsigned int i;
    enum v4l2_buf_type type;
    BOOST_ASSERT(io == IO_METHOD_MMAP);

    switch (io) {
    case IO_METHOD_READ:
      /* Nothing to do. */
      break;

    case IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
          errno_exit("VIDIOC_QBUF");
      }
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
      break;

    case IO_METHOD_USERPTR:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long)buffers[i].start;
        buf.length = buffers[i].length;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
          errno_exit("VIDIOC_QBUF");
      }
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
      break;
    }
  }
  void mainloop() {
    read_thread = std::thread([this] {
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
          errno_exit("select");
        }

        if (0 == result) {
          fprintf(stderr, "select timeout\\n");
          exit(EXIT_FAILURE);
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
    unsigned int i;

    BOOST_ASSERT(io == IO_METHOD_MMAP);
    switch (io) {
    case IO_METHOD_READ:
      if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
        switch (errno) {
        case EAGAIN:
          return 0;

        case EIO:
          /* Could ignore EIO, see spec. */

          /* fall through */

        default:
          errno_exit("read");
        }
      }

      process_image(buffers[0].start, buffers[0].length);
      break;

    case IO_METHOD_MMAP:
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
          errno_exit("VIDIOC_DQBUF");
        }
      }

      assert(buf.index < n_buffers);

      process_image(buffers[buf.index].start, buf.bytesused);

      if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
      break;

    case IO_METHOD_USERPTR:
      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;

      if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
        case EAGAIN:
          return 0;

        case EIO:
          /* Could ignore EIO, see spec. */

          /* fall through */

        default:
          errno_exit("VIDIOC_DQBUF");
        }
      }

      for (i = 0; i < n_buffers; ++i)
        if (buf.m.userptr == (unsigned long)buffers[i].start &&
            buf.length == buffers[i].length)
          break;

      assert(i < n_buffers);

      process_image((void *)buf.m.userptr, buf.bytesused);

      if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
      break;
    }

    return 1;
  }

  void process_image(const void *p, int size) {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", size: " << size;
#endif
    on_data(p, size);
  }
};

class video_encoder_impl : public video_encoder {
public:
  webrtc::EncodedImageCallback *callback{};
  webrtc::Clock *const clock_{webrtc::Clock::GetRealTimeClock()};
  const config config_;
  std::unique_ptr<v4l2_h264_reader> reader;

  video_encoder_impl(const config &config_) : config_{config_} {}

  void SetFecControllerOverride(
      webrtc::FecControllerOverride *fec_controller_override) override {
    (void)fec_controller_override;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  }

  int InitEncode(const webrtc::VideoCodec *codec_settings,
                 const VideoEncoder::Settings &settings) override {
#if 0
    // TODO
    codec_settings->startBitrate;
#endif
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
        << __FUNCTION__ << ", init_device";
    reader->init_device();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", start_capturing";
    reader->start_capturing();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", main_loop";
    reader->mainloop();
    return WEBRTC_VIDEO_CODEC_OK;
  }

  void on_data(const void *data, int size) {
    // TODO here we r using a deprecated method
    webrtc::EncodedImage encoded{
        static_cast<std::uint8_t *>(const_cast<void *>(data)),
        static_cast<std::size_t>(size), static_cast<std::size_t>(size)};
    webrtc::CodecSpecificInfo info;
    webrtc::CodecSpecificInfoH264 codec_info;
    codec_info.packetization_mode =
        webrtc::H264PacketizationMode::NonInterleaved;
    codec_info.temporal_idx = webrtc::kNoTemporalIdx;
    // TODO detect i-frame (intra frame)
    // https://stackoverflow.com/questions/1957427/detect-mpeg4-h264-i-frame-idr-in-rtp-stream
    auto frameType = webrtc::VideoFrameType::kVideoFrameDelta;
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
    int64_t ntp_capture_time_ms = clock_->CurrentNtpInMilliseconds();

    encoded.SetTimestamp(capture_time_ms);
    encoded.SetSpatialIndex(0);
    encoded.ntp_time_ms_ = ntp_capture_time_ms;
    encoded.capture_time_ms_ = capture_time_ms;
    encoded._encodedWidth = config_.width;
    encoded._encodedHeight = config_.height;

    info.codecSpecific.H264 = codec_info;
    info.codecType = webrtc::kVideoCodecH264;
    // info.end_of_picture = false;
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
          << __FUNCTION__ << "error in passing EncodedImage, ERROR_SEND_FAILED";
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
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int32_t
  Encode(const webrtc::VideoFrame &frame,
         const std::vector<webrtc::VideoFrameType> *frame_types) override {
    (void)frame;
    (void)frame_types;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    return WEBRTC_VIDEO_CODEC_OK;
  }

  void SetRates(const RateControlParameters &parameters) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", framerate_fps: " << parameters.framerate_fps
        << ", bandwidth_allocation: " << parameters.bandwidth_allocation.kbps()
        << ", bitrate: " << parameters.bitrate.ToString()
        << ", target_bitrate: " << parameters.target_bitrate.ToString();
  }

  EncoderInfo GetEncoderInfo() const override {
    auto info = webrtc::VideoEncoder::GetEncoderInfo();
    info.has_internal_source = true;
    info.is_hardware_accelerated = true;
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
