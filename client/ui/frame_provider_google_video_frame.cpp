#include "frame_provider_google_video_frame.hpp"
#include "thread_helper.hpp"
#include <fmt/format.h>

using namespace client::ui;

namespace {

class video_buffer_adapter : public QAbstractPlanarVideoBuffer {
public:
  video_buffer_adapter(const webrtc::VideoFrame &frame)
      : QAbstractPlanarVideoBuffer(NoHandle), frame(frame) {
    webrtc::VideoFrameBuffer *buffer = frame.video_frame_buffer().get();
    if (buffer->type() == webrtc::VideoFrameBuffer::Type::kI420) {
      buffer_accessor = buffer->GetI420();
      return;
    }
    i420_reference = buffer->ToI420();
    buffer_accessor = i420_reference.get();
  }
  ~video_buffer_adapter() = default;

  MapMode mapMode() const override { return ReadOnly; }

  int map([[maybe_unused]] MapMode mode, int *numBytes, int bytesPerLine[4],
          uchar *data[4]) override {
    BOOST_ASSERT(mode == ReadOnly);
    data[0] = const_cast<uchar *>(buffer_accessor->DataY());
    data[1] = const_cast<uchar *>(buffer_accessor->DataU());
    data[2] = const_cast<uchar *>(buffer_accessor->DataV());
    bytesPerLine[0] = buffer_accessor->StrideY();
    bytesPerLine[1] = buffer_accessor->StrideU();
    bytesPerLine[2] = buffer_accessor->StrideV();
    // numBytes should contain the summed buffer size. Works withouth setting
    // it.
    *numBytes = 0;
    return 3;
  }

  void unmap() override {}

private:
  logging::logger logger;
  webrtc::VideoFrame frame;
  const webrtc::I420BufferInterface *buffer_accessor;
  rtc::scoped_refptr<webrtc::I420BufferInterface> i420_reference;
};
} // namespace

frame_provider_google_video_source::frame_provider_google_video_source() =
    default;

frame_provider_google_video_source::~frame_provider_google_video_source() {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << " ~frame_provider_google_video_source()";
}

void frame_provider_google_video_source::set_source(
    rtc::google::video_source *source_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "set_source, source_:" << source_;
  connection_on_frame.disconnect();
  source = source_;
  if (!source)
    return;
  connection_on_frame =
      source->on_frame.connect([this](const auto &frame) { on_frame(frame); });
}

void frame_provider_google_video_source::set_surface(
    QAbstractVideoSurface *surface_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "set_surface, surface:" << surface_;
  if (surface_ != surface && surface && surface->isActive())
    surface->stop();
  surface = surface_;
}

QAbstractVideoSurface *frame_provider_google_video_source::get_surface() const {
  return surface;
}

void frame_provider_google_video_source::on_frame(
    const webrtc::VideoFrame &frame) {
  auto frame_buffer = frame.video_frame_buffer();
  webrtc::VideoFrameBuffer::Type type = frame_buffer->type();
#if 1
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "frame, width:" << frame.width() << ", height:" << frame.height()
      << ", type:" << static_cast<int>(type);
#endif
  BOOST_ASSERT(type == webrtc::VideoFrameBuffer::Type::kI420);
  // destruction happens through VideoFrame
  // https://doc.qt.io/qt-5/qabstractvideobuffer.html#release
  auto frame_buffer_casted = new video_buffer_adapter(frame);
  QSize size{frame_buffer->width(), frame_buffer->height()};
  QVideoFrame frame_casted{frame_buffer_casted, size,
                           QVideoFrame::Format_YUV420P};
  post_to_object([frame_casted, this] { on_frame_ui_thread(frame_casted); }, this);
}

void frame_provider_google_video_source::on_frame_ui_thread(
    const QVideoFrame &frame) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_frame_ui_thread";
  if (!surface) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "on_frame_ui_thread a frame gets discarded because no surface "
           "is set";
    return;
  }
  set_format(frame.size());
  start_surface();
  surface->present(frame);
}

void frame_provider_google_video_source::set_format(const QSize &set) {
  if (!surface)
    return;
  if (set == current_frame_size)
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug) << fmt::format(
      "set_size, width: {}, height: {}", set.width(), set.height());
  current_frame_size = set;
  auto pixel_format = QVideoFrame::Format_YUV420P;
  format = QVideoSurfaceFormat(set, pixel_format);
  if (!surface)
    return;
  if (surface->isActive())
    surface->stop();
  // format = surface->nearestFormat(format);
}

void frame_provider_google_video_source::start_surface() {
  if (!surface)
    return;
  if (format != surface->surfaceFormat())
    surface->stop();
  if (surface->isActive())
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting surface";
  const bool success = surface->start(format);
  BOOST_ASSERT(success);
  if (success)
    return;
  throw std::runtime_error(
      fmt::format("could not set format: '{}'", format.pixelFormat()));
}
