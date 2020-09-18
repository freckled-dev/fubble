#include "capturer.hpp"
#include "exception.hpp"
#include "rtc/logger.hpp"
#include <boost/assert.hpp>
#include <modules/desktop_capture/desktop_capture_options.h>
#include <modules/desktop_capture/desktop_capturer.h>

using namespace rtc::google::capture::desktop;

namespace {
struct could_not_select_source : utils::exception {};
using desktop_id_info = boost::error_info<struct desktop_id_tag, std::intptr_t>;

class capturer_impl : public capturer,
                      public webrtc::DesktopCapturer::Callback {
public:
  capturer_impl(std::unique_ptr<webrtc::DesktopCapturer> delegate_move,
                std::intptr_t id)
      : delegate{std::move(delegate_move)} {
    if (!delegate->SelectSource(id))
      BOOST_THROW_EXCEPTION(could_not_select_source() << desktop_id_info(id));
  }

protected:
  boost::future<void> start() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    BOOST_ASSERT(!start_promise);
    start_promise = std::make_unique<boost::promise<void>>();
    delegate->Start(this);
    return start_promise->get_future();
  }

  void stop() override {}

  void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                       std::unique_ptr<webrtc::DesktopFrame> frame) override {
    if (result == webrtc::DesktopCapturer::Result::ERROR_PERMANENT) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << ", ERROR_PERMANENT";
      return;
    }
  }

  std::unique_ptr<webrtc::DesktopCapturer> delegate;
  std::unique_ptr<boost::promise<void>> start_promise;
};
} // namespace

std::unique_ptr<capturer> capturer::create_screen(std::intptr_t id) {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
  auto desktop_capturer =
      webrtc::DesktopCapturer::CreateScreenCapturer(options);
  return std::make_unique<capturer_impl>(std::move(desktop_capturer), id);
}

std::unique_ptr<capturer> capturer::create_window(std::intptr_t id) {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
  auto window_capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
  return std::make_unique<capturer_impl>(std::move(window_capturer), id);
}
