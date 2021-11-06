#include "reader.hpp"
#include <sigslot/signal.hpp>
#include <fmt/format.h>
#include <fubble/rtc/logger.hpp>
#include <fubble/utils/exception.hpp>
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
class reader_impl : public reader {
public:
  struct buffer {
    void *start{};
    size_t length{};
  };
  using buffers_type = std::vector<buffer>;

  rtc::logger logger{"reader"};
  const config config_;
  const std::string &device = config_.path;
  const char *device_c = device.c_str();
  int fd{-1};
  buffers_type buffers;
  std::thread read_thread;
  bool run{false};
  std::unordered_set<int> available_ctrls;

  reader_impl(const config &config_) : config_{config_} {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  void initialise(unsigned int start_bitrate) override {
    if (fd >= 0)
      return;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", open_device";
    open_device();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", enumerate_ctrls";
    enumerate_ctrls();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", init_device";
    init_device();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", set_bitrate";
    set_bitrate(start_bitrate);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", start_capturing";
    start_capturing();
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", mainloop";
    mainloop();
#endif
  }

  ~reader_impl() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
    stop();
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
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__
          << fmt::format(
                 ", setting format, width: {}, height: {}, framerate: {}",
                 config_.width, config_.height, config_.framerate);
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

    } else {
      // Preserve original settings as set by v4l2-ctl for example
      xioctl_throw(fd, VIDIOC_G_FMT, &fmt, "VIDIOC_G_FMT");
    }

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
    } else {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__
          << ", V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER is not available.";
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

  void trigger_i_frame() override {
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

  void mainloop() override {
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

  void set_bitrate(int bitrate) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", bitrate: " << bitrate;

    if (available_ctrls.find(V4L2_CID_MPEG_VIDEO_BITRATE) ==
        available_ctrls.cend()) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__
          << ", V4L2_CID_MPEG_VIDEO_BITRATE is not available. Won't be able to "
             "set bitrate";
      return;
    }

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

  void stop() {
    stop_capturing();
    uninit_device();
    close_device();
  }
};

class reader_shared : public reader {
public:
  reader_shared(std::shared_ptr<reader> delegate) : delegate{delegate} {
    on_data_connection = delegate->on_data.connect(
        [this](auto data, auto size) { on_data(data, size); });
  }

  void initialise(unsigned int start_bitrate) override {
    delegate->initialise(start_bitrate);
  }

  void set_bitrate(int bitrate) override {
    // TODO do something smart? multiple call sources with different bitrates
    delegate->set_bitrate(bitrate);
  }
  void trigger_i_frame() override { delegate->trigger_i_frame(); }

  void mainloop() override { delegate->mainloop(); }

private:
  std::shared_ptr<reader> delegate;
  sigslot::scoped_connection on_data_connection;
};
} // namespace

std::unique_ptr<reader> reader::create(config config_) {
  return std::make_unique<reader_impl>(config_);
}

std::unique_ptr<reader> reader::create_shared(std::shared_ptr<reader> reader_) {
  return std::make_unique<reader_shared>(reader_);
}
