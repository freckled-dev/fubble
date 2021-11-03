#include "capturer.hpp"
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>
#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <fmt/format.h>
#include <fubble/rtc/google/video_frame.hpp>
#include <fubble/rtc/logger.hpp>
#include <fubble/utils/exception.hpp>
#include <fubble/utils/timer.hpp>
#include <libyuv.h>
#include <modules/desktop_capture/desktop_and_cursor_composer.h>
#include <modules/desktop_capture/desktop_capture_options.h>
#include <modules/desktop_capture/desktop_capturer.h>

using namespace rtc::google::capture::desktop;

namespace {
struct could_not_select_source : utils::exception {};
struct error_temporary : utils::exception {};
struct error_permanent : utils::exception {};
using desktop_id_info = boost::error_info<struct desktop_id_tag, std::intptr_t>;

class capturer_video_source_adapter : public rtc::google::video_source,
                                      public webrtc::DesktopCapturer::Callback {
public:
  capturer_video_source_adapter(
      std::shared_ptr<webrtc::DesktopCapturer> delegate)
      : delegate{delegate} {}

  boost::future<void> capture() {
    BOOST_LOG_SEV(logger, logging::severity::trace) << __FUNCTION__;
    BOOST_ASSERT(!capture_promise);
    capture_promise = std::make_shared<boost::promise<void>>();
    auto promise_reference = capture_promise;
    delegate->CaptureFrame();
    return promise_reference->get_future();
  }

protected:
  void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                       std::unique_ptr<webrtc::DesktopFrame> frame) override {
    BOOST_LOG_SEV(logger, logging::severity::trace)
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
    if (i420_buffer.get() == nullptr)
      i420_buffer = webrtc::I420Buffer::Create(width, height);
    if (i420_buffer->width() != width || i420_buffer->height() != height)
      i420_buffer = webrtc::I420Buffer::Create(width, height);
    libyuv::ARGBToI420(frame->data(), frame->stride(),
                       i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                       i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                       i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                       width, height);
    webrtc::VideoFrame::Builder builder;
    builder.set_video_frame_buffer(i420_buffer);
    webrtc::VideoFrame casted_frame = builder.build();
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << __FUNCTION__ << ", after convert to i420";
    rtc::google::video_frame casted_again{casted_frame};
    on_frame(casted_again);
    promise_reference->set_value();
  }

protected:
  rtc::logger logger{"capturer_video_source_adapter"};
  std::shared_ptr<boost::promise<void>> capture_promise;
  std::shared_ptr<webrtc::DesktopCapturer> delegate;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer;
};

class capturer_impl final : public capturer {
public:
  capturer_impl(std::unique_ptr<webrtc::DesktopCapturer> delegate_move,
                std::intptr_t id, std::string title)
      : logger{fmt::format("capturer_impl:{}:{}", id, title)},
        delegate{std::move(delegate_move)}, id{id}, title{title} {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    source_adapter = std::make_shared<capturer_video_source_adapter>(delegate);
    if (!delegate->SelectSource(id))
      BOOST_THROW_EXCEPTION(could_not_select_source() << desktop_id_info(id));
    delegate->Start(source_adapter.get());
  }

protected:
  boost::future<void> capture() override {
    BOOST_LOG_SEV(logger, logging::severity::trace) << __FUNCTION__;
    return source_adapter->capture();
  }

  std::shared_ptr<rtc::google::video_source> get_source() const override {
    return source_adapter;
  }

  std::intptr_t get_id() override { return id; }

  std::string get_title() override { return title; }

  rtc::logger logger;
  std::shared_ptr<webrtc::DesktopCapturer> delegate;
  const std::intptr_t id;
  const std::string title;
  std::shared_ptr<capturer_video_source_adapter> source_adapter;
};

class interval_capturer_impl final : public interval_capturer {
public:
  interval_capturer_impl(std::unique_ptr<utils::interval_timer> timer,
                         std::unique_ptr<capturer> delegate_)
      : timer{std::move(timer)}, delegate{std::move(delegate_)} {}

  boost::future<void> start() override {
    BOOST_ASSERT(!start_promise);
    start_promise = std::make_shared<boost::promise<void>>();
    timer->start_immediately([this] { on_timeout(); });
    return start_promise->get_future();
  }

  void stop() override {
    BOOST_ASSERT(start_promise);
    timer->stop();
    decltype(start_promise) moved = std::move(start_promise);
    moved->set_value();
  }

  bool get_started() const override { return start_promise != nullptr; }

  void on_timeout() {
    delegate->capture().then(executor,
                             [this](auto result) { on_captured(result); });
  }

  void on_captured(boost::future<void> &result) {
    BOOST_ASSERT(start_promise);
    try {
      result.get();
    } catch (const error_temporary &) {
      // ignore
    } catch (...) {
      timer->stop();
      decltype(start_promise) moved = std::move(start_promise);
      moved->set_exception(boost::current_exception());
    }
  }

  capturer &get_capturer() override { return *delegate; }

protected:
  boost::inline_executor executor;
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

enum class type { screen, window };

webrtc::DesktopCaptureOptions make_options() {
  return webrtc::DesktopCaptureOptions::CreateDefault();
}

boost::optional<type> get_type(std::intptr_t id) {
  webrtc::DesktopCaptureOptions options = make_options();
  {
    auto screen_capturer =
        webrtc::DesktopCapturer::CreateScreenCapturer(options);
    webrtc::DesktopCapturer::SourceList sources;
    if (!screen_capturer->GetSourceList(&sources))
      return {};
    auto found = std::find_if(sources.cbegin(), sources.cend(),
                              [&](auto &check) { return check.id == id; });
    if (found != sources.cend())
      return type::screen;
  }
  {
    auto window_capturer =
        webrtc::DesktopCapturer::CreateWindowCapturer(options);
    webrtc::DesktopCapturer::SourceList sources;
    if (!window_capturer->GetSourceList(&sources))
      return {};
    auto found = std::find_if(sources.cbegin(), sources.cend(),
                              [&](auto &check) { return check.id == id; });
    if (found != sources.cend())
      return type::window;
  }
  return {};
} // namespace
std::unique_ptr<webrtc::DesktopCapturer>
wrap_capturer_with_cursor(std::unique_ptr<webrtc::DesktopCapturer> wrap) {
  auto options = make_options();
  auto result = std::make_unique<webrtc::DesktopAndCursorComposer>(
      std::move(wrap), options);
  return result;
}
} // namespace

std::unique_ptr<capturer> capturer::create(std::intptr_t id, bool cursor) {
  auto type_ = get_type(id);
  if (type_ == type::screen)
    return create_screen(id, cursor);
  return create_window(id, cursor);
}

std::unique_ptr<capturer> capturer::create_with_cursor(std::intptr_t id) {
  return create(id, true);
}

std::unique_ptr<capturer> capturer::create_without_cursor(std::intptr_t id) {
  return create(id, false);
}

std::unique_ptr<capturer> capturer::create_screen(std::intptr_t id,
                                                  bool cursor) {
  webrtc::DesktopCaptureOptions options = make_options();
  auto desktop_capturer =
      webrtc::DesktopCapturer::CreateScreenCapturer(options);
  if (cursor)
    desktop_capturer = wrap_capturer_with_cursor(std::move(desktop_capturer));
  std::string title = get_title_by_id(*desktop_capturer, id).value();
  return std::make_unique<capturer_impl>(std::move(desktop_capturer), id,
                                         title);
}

std::unique_ptr<capturer> capturer::create_window(std::intptr_t id,
                                                  bool cursor) {
  webrtc::DesktopCaptureOptions options = make_options();
  auto window_capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
  if (cursor)
    window_capturer = wrap_capturer_with_cursor(std::move(window_capturer));
  std::string title = get_title_by_id(*window_capturer, id).value();
  return std::make_unique<capturer_impl>(std::move(window_capturer), id, title);
}

std::unique_ptr<interval_capturer>
interval_capturer::create(std::unique_ptr<utils::interval_timer> timer,
                          std::unique_ptr<capturer> delegate) {
  return std::make_unique<interval_capturer_impl>(std::move(timer),
                                                  std::move(delegate));
}
