#include "frame_provider_google_video_frame.hpp"
#include "thread_helper.hpp"
#include <fmt/format.h>

using namespace client::ui;

namespace {

class video_buffer_adapter : public QAbstractPlanarVideoBuffer {
public:
  video_buffer_adapter(const rtc::video_frame &frame)
      : QAbstractPlanarVideoBuffer(NoHandle) {
    frame.i420(data_y, stride_y, data_u, stride_u, data_v, stride_v);
  }
  ~video_buffer_adapter() = default;

  MapMode mapMode() const override { return ReadOnly; }

  int map([[maybe_unused]] MapMode mode, int *numBytes, int bytesPerLine[4],
          uchar *data[4]) override {
    BOOST_ASSERT(mode == ReadOnly);
    data[0] = data_y.data();
    data[1] = data_u.data();
    data[2] = data_v.data();
    bytesPerLine[0] = stride_y;
    bytesPerLine[1] = stride_u;
    bytesPerLine[2] = stride_v;
    // numBytes should contain the summed buffer size. Works without setting
    // it.
    *numBytes = 0;
    return 3;
  }

  void unmap() override {}

private:
  client::logger logger{"video_buffer_adapter"};
  std::vector<uchar> data_y;
  int stride_y{};
  std::vector<uchar> data_u;
  int stride_u{};
  std::vector<uchar> data_v;
  int stride_v{};
};
} // namespace

frame_provider_google_video_source::frame_provider_google_video_source(
    QObject *parent)
    : QObject(parent) {}

frame_provider_google_video_source::~frame_provider_google_video_source() {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << " ~frame_provider_google_video_source()";
}

void frame_provider_google_video_source::set_source(
    std::shared_ptr<rtc::video_source> source_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "set_source, source_:" << source_;
  connection_on_frame.disconnect();
  source = source_;
  if (!source)
    return;
  connection_on_frame =
      source->on_frame.connect([this](const auto &frame) { on_frame(frame); });
}

std::shared_ptr<rtc::video_source>
frame_provider_google_video_source::get_source() const {
  return source;
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
    const rtc::video_frame &frame) {
  // destruction happens through VideoFrame
  // https://doc.qt.io/qt-5/qabstractvideobuffer.html#release
  auto frame_buffer_casted = new video_buffer_adapter(frame);
  QSize size{frame.width(), frame.height()};
  QVideoFrame frame_casted{frame_buffer_casted, size,
                           QVideoFrame::Format_YUV420P};
  post_to_object([frame_casted, this] { on_frame_ui_thread(frame_casted); },
                 this);
}

void frame_provider_google_video_source::on_frame_ui_thread(
    const QVideoFrame &frame) {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_frame_ui_thread";
#endif
  if (!surface) {
    BOOST_LOG_SEV(logger, logging::severity::trace)
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
