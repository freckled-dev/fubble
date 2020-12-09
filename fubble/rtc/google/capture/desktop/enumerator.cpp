#include "enumerator.hpp"
#include "fubble/rtc/logger.hpp"
#include <boost/assert.hpp>
#include <modules/desktop_capture/desktop_capture_options.h>
#include <modules/desktop_capture/desktop_capturer.h>

using namespace rtc::google::capture::desktop;

namespace {
class enumerator_impl : public enumerator {
public:
  enumerator_impl() {
    webrtc::DesktopCaptureOptions options =
        webrtc::DesktopCaptureOptions::CreateDefault();
    window_capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
    desktop_capturer = webrtc::DesktopCapturer::CreateScreenCapturer(options);
  }

  void enumerate() override {
    screens.clear();
    windows.clear();
    auto cast_source_to_information =
        [](const webrtc::DesktopCapturer::Source &cast) {
          return information{cast.id, cast.title};
        };
    {
      webrtc::DesktopCapturer::SourceList sources;
      bool result = window_capturer->GetSourceList(&sources);
      BOOST_ASSERT(result);
      if (!result) {
        BOOST_LOG_SEV(logger, logging::severity::error)
            << __FUNCTION__ << "window_capturer !result";
        return;
      }
      std::transform(sources.cbegin(), sources.cend(),
                     std::back_inserter(windows), cast_source_to_information);
    }
    {
      webrtc::DesktopCapturer::SourceList sources;
      bool result = desktop_capturer->GetSourceList(&sources);
      BOOST_ASSERT(result);
      if (!result) {
        BOOST_LOG_SEV(logger, logging::severity::error)
            << __FUNCTION__ << "desktop_capturer !result";
        return;
      }
      std::transform(sources.cbegin(), sources.cend(),
                     std::back_inserter(screens), cast_source_to_information);
    }
  }

  std::vector<information> get_screens() const override { return screens; }

  std::vector<information> get_windows() const override { return windows; }

protected:
  rtc::logger logger{"enumerator_impl"};
  std::unique_ptr<webrtc::DesktopCapturer> window_capturer;
  std::unique_ptr<webrtc::DesktopCapturer> desktop_capturer;
  std::vector<information> windows;
  std::vector<information> screens;
};
} // namespace

std::unique_ptr<enumerator> enumerator::create() {
  return std::make_unique<enumerator_impl>();
}
