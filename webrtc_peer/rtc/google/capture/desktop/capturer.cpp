#include "capturer.hpp"
#include "exception.hpp"
#include "rtc/logger.hpp"
#include "timer.hpp"
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>
#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <libyuv.h>
#include <modules/desktop_capture/desktop_capture_options.h>
#include <modules/desktop_capture/desktop_capturer.h>

using namespace rtc::google::capture::desktop;

namespace {
struct could_not_select_source : utils::exception {};
struct error_temporary : utils::exception {};
struct error_permanent : utils::exception {};
using desktop_id_info = boost::error_info<struct desktop_id_tag, std::intptr_t>;

class capturer_impl final : public capturer,
                            public webrtc::DesktopCapturer::Callback {
public:
  capturer_impl(std::unique_ptr<webrtc::DesktopCapturer> delegate_move,
                std::intptr_t id, std::string title)
      : delegate{std::move(delegate_move)}, id{id}, title{title} {
    if (!delegate->SelectSource(id))
      BOOST_THROW_EXCEPTION(could_not_select_source() << desktop_id_info(id));
    delegate->Start(this);
  }

protected:
  boost::future<void> capture() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    BOOST_ASSERT(!capture_promise);
    capture_promise = std::make_shared<boost::promise<void>>();
    auto promise_reference = capture_promise;
    delegate->CaptureFrame();
    return promise_reference->get_future();
  }

  std::intptr_t get_id() override { return id; }

  std::string get_title() override { return title; }

  void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                       std::unique_ptr<webrtc::DesktopFrame> frame) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", result:" << static_cast<int>(result);
    auto promise_reference = std::move(capture_promise);
    if (result == webrtc::DesktopCapturer::Result::ERROR_PERMANENT) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << ", ERROR_PERMANENT";
      promise_reference->set_exception(error_permanent());
      return;
    }
    if (result == webrtc::DesktopCapturer::Result::ERROR_TEMPORARY) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << ", ERROR_TEMPORARY";
      promise_reference->set_exception(error_temporary());
      return;
    }
    auto width = frame->size().width();
    auto height = frame->size().height();
    if (!i420_buffer.get() ||
        i420_buffer->width() * i420_buffer->height() < width * height)
      i420_buffer = webrtc::I420Buffer::Create(width, height);
    libyuv::ConvertToI420(frame->data(), 0, i420_buffer->MutableDataY(),
                          i420_buffer->StrideY(), i420_buffer->MutableDataU(),
                          i420_buffer->StrideU(), i420_buffer->MutableDataV(),
                          i420_buffer->StrideV(), 0, 0, width, height, width,
                          height, libyuv::kRotate0, libyuv::FOURCC_ARGB);
    webrtc::VideoFrame::Builder builder;
    builder.set_video_frame_buffer(i420_buffer);
    webrtc::VideoFrame casted_frame = builder.build();
    on_frame(casted_frame);
    promise_reference->set_value();
  }

  rtc::logger logger{"capturer_impl"};
  std::unique_ptr<webrtc::DesktopCapturer> delegate;
  const std::intptr_t id;
  const std::string title;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer;
  std::shared_ptr<boost::promise<void>> capture_promise;
};

class interval_capturer_impl final : public interval_capturer {
public:
  interval_capturer_impl(std::unique_ptr<utils::interval_timer> timer,
                         std::unique_ptr<capturer> delegate_)
      : timer{std::move(timer)}, delegate{std::move(delegate_)} {}

  boost::future<void> start() override {
    BOOST_ASSERT(!start_promise);
    start_promise = std::make_shared<boost::promise<void>>();
    timer->start([this] { on_timeout(); });
    return start_promise->get_future();
  }

  void stop() override {
    BOOST_ASSERT(start_promise);
    timer->stop();
    start_promise->set_value();
  }

  void on_timeout() { delegate->capture(); }

  capturer &get_capturer() override { return *delegate; }

protected:
  std::unique_ptr<utils::interval_timer> timer;
  std::unique_ptr<capturer> delegate;
  std::shared_ptr<boost::promise<void>> start_promise;
};
boost::optional<std::string> get_title_by_id(webrtc::DesktopCapturer &capturer,
                                             std::intptr_t id) {
  webrtc::DesktopCapturer::SourceList sources;
  if (!capturer.GetSourceList(&sources))
    return {};
  auto found = std::find_if(sources.cbegin(), sources.cend(),
                            [&](auto &check) { return check.id == id; });
  if (found == sources.cend())
    return {};
  return found->title;
}
} // namespace

std::unique_ptr<capturer> capturer::create_screen(std::intptr_t id) {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
  auto desktop_capturer =
      webrtc::DesktopCapturer::CreateScreenCapturer(options);
  std::string title = get_title_by_id(*desktop_capturer, id).value();
  return std::make_unique<capturer_impl>(std::move(desktop_capturer), id,
                                         title);
}

std::unique_ptr<capturer> capturer::create_window(std::intptr_t id) {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
  auto window_capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
  std::string title = get_title_by_id(*window_capturer, id).value();
  return std::make_unique<capturer_impl>(std::move(window_capturer), id, title);
}

std::unique_ptr<interval_capturer>
interval_capturer::create(std::unique_ptr<utils::interval_timer> timer,
                          std::unique_ptr<capturer> delegate) {
  return std::make_unique<interval_capturer_impl>(std::move(timer),
                                                  std::move(delegate));
}
