#include "cli_client.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/predef/os/linux.h>
#include <fmt/format.h>
#include <fubble/client/joiner.hpp>
#include <fubble/client/leaver.hpp>
#include <fubble/client/own_media.hpp>
#include <fubble/client/peer.hpp>
#include <fubble/client/peers.hpp>
#include <fubble/client/session_module.hpp>
#include <fubble/client/video_module.hpp>
#include <fubble/client/video_settings.hpp>
#include <fubble/rtc/google/log_webrtc_to_logging.hpp>
#include <fubble/rtc/google/module.hpp>
#include <fubble/utils/timer.hpp>
#include <fubble/utils/uuid.hpp>
#if FUBBLE_WITH_V4L2_HW_H264
#include <fubble/v4l2_hw_h264/module.hpp>
#endif
#include <nlohmann/json.hpp>

using namespace fubble;

#if FUBBLE_WITH_V4L2_HW_H264
namespace fubble::v4l2_hw_h264 {
class rtc_module : public rtc::google::module {
public:
  rtc_module(std::shared_ptr<utils::executor_module> executor_module,
             const rtc::google::settings rtc_settings, const config &config_)
      : rtc::google::module(executor_module, rtc_settings), config_{config_} {}

  std::shared_ptr<rtc::video_device_factory>
  get_video_device_creator() override {
    if (!video_device_creator)
      video_device_creator = std::make_shared<video_device_factory>();
    return video_device_creator;
  }

  std::shared_ptr<rtc::google::video_encoder_factory_factory>
  get_video_encoder_factory_factory() override {
    if (!video_encoder_factory_factory_)
      video_encoder_factory_factory_ =
          std::make_shared<video_encoder_factory_factory>(config_);
    return video_encoder_factory_factory_;
  }

protected:
  const config config_;
};
class core_module : public client::core_module {
public:
  core_module(const core_module::config &core_config_,
              const v4l2_hw_h264::config &encoder_config_)
      : client::core_module(core_config_), encoder_config_{encoder_config_} {}

  std::shared_ptr<rtc::module> get_rtc_module() override {
    if (!rtc_module)
      rtc_module = std::make_shared<v4l2_hw_h264::rtc_module>(
          get_utils_executor_module(), config_.rtc_, encoder_config_);
    return rtc_module;
  }

  const v4l2_hw_h264::config encoder_config_;
};
} // namespace fubble::v4l2_hw_h264
#endif

namespace {
class cli_client_impl : public fubble::cli_client {
public:
  cli_client_impl(const config &config_) : config_{config_} {
    // core
    bool use_v4l2_hw_h264 = config_.use_v4l2_hw_h264;
#if !FUBBLE_WITH_V4L2_HW_H264
    BOOST_ASSERT(use_v4l2_hw_h264 == false);
    use_v4l2_hw_h264 = false;
#endif
    if (use_v4l2_hw_h264) {
#if FUBBLE_WITH_V4L2_HW_H264
      core = std::make_shared<v4l2_hw_h264::core_module>(
          config_.core, config_.v4l2_hw_h264_config);
#endif
    } else {
      core = std::make_shared<client::core_module>(config_.core);
    }

    // audio
    auto client_audio_module = std::make_shared<client::audio_module>(
        core->get_utils_executor_module(),
        std::dynamic_pointer_cast<rtc::google::module>(core->get_rtc_module()),
        config_.audio);
    core->get_session_module()->get_own_media()->set_own_audio_track(
        client_audio_module->get_own_audio_track());
    audio_settings = std::make_shared<client::audio_settings_module>(
        core->get_utils_executor_module(),
        std::dynamic_pointer_cast<rtc::google::module>(core->get_rtc_module()),
        client_audio_module, core->get_session_module(),
        config_.audio_settings);
    // TODO instancition adds the audio track to the tracks_adder. refactor!
    if (config_.send_audio)
      audio_settings->get_audio_tracks_volume();

    // video
    client_video = std::make_shared<client::video_module>(
        core->get_utils_executor_module(), core->get_rtc_module(),
        core->get_session_module(), config_.video);
    // TODO refactor #355
#if 0
    core->get_session_module()->get_own_media()->set_own_video(
        client_video->get_own_video());
#endif
    // TODO instancition of video_settings adds the video track to the
    // tracks_adder. refactor!
    auto video_settings = client_video->get_video_settings();
    video_settings->pause(!config_.send_video);
  }

private:
  int run() override {
    auto joiner = core->get_session_module()->get_joiner();
    client::joiner::parameters parameters{uuid::generate(), config_.room_name};
    joiner->join(parameters).then(executor, [this](auto result) {
      try {
        result.get();
      } catch (...) {
        run_promise.set_exception(boost::current_exception());
        stop_execution();
      }
    });
    try {
      core->get_utils_executor_module()->get_io_context()->run();
    } catch (const boost::exception &) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << " "
          << boost::current_exception_diagnostic_information();
      run_promise.set_exception(boost::current_exception());
    }
    return run_promise.get_future().get();
  }

  void stop_execution() {
    core->get_utils_executor_module()->get_io_context()->stop();
  }

  void stop() override {
    core->get_session_module()->get_leaver()->leave().then([this](auto result) {
      try {
        result.get();
        run_promise.set_value(0);
      } catch (...) {
        run_promise.set_exception(boost::current_exception());
      }
      stop_execution();
    });
  }

  std::shared_ptr<client::core_module> get_core() const override {
    return core;
  }

  const config config_;
  logging::logger logger{"cli"};
  rtc::google::log_webrtc_to_logging log_webrtc_to_logging;
  boost::inline_executor executor;
  std::shared_ptr<client::core_module> core;
  std::shared_ptr<client::audio_settings_module> audio_settings;
  std::shared_ptr<client::video_module> client_video;
  boost::promise<int> run_promise;
};
} // namespace

std::unique_ptr<fubble::cli_client>
cli_client::cli_client::create(const config &config_) {
  return std::make_unique<cli_client_impl>(config_);
}
