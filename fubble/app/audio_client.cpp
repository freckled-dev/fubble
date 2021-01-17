#include "audio_client.hpp"
#include "fubble/client/joiner.hpp"
#include "fubble/client/leaver.hpp"
#include "fubble/client/own_media.hpp"
#include "fubble/client/peer.hpp"
#include "fubble/client/peers.hpp"
#include "fubble/client/session_module.hpp"
#include "fubble/rtc/google/log_webrtc_to_logging.hpp"
#include "fubble/utils/timer.hpp"
#include "fubble/utils/uuid.hpp"
#include <boost/asio/io_context.hpp>

namespace {
class audio_client_impl : public audio_client::audio_client {
public:
  audio_client_impl(const config &config_) : config_{config_} {
    core = std::make_shared<client::core_module>(config_.core);
    auto client_audio_module = std::make_shared<client::audio_module>(
        core->get_utils_executor_module(), core->get_rtc_module(),
        config_.audio);
    core->get_session_module()->get_own_media()->set_own_audio_track(
        client_audio_module->get_own_audio_track());
    audio_settings = std::make_shared<client::audio_settings_module>(
        core->get_utils_executor_module(), core->get_rtc_module(),
        client_audio_module, core->get_session_module(),
        config_.audio_settings);
    // TODO instancition adds the audio track to the tracks_adder. refactor!
    if (config_.send_audio)
      audio_settings->get_audio_tracks_volume();
    stats_timer = core->get_utils_executor_module()
                      ->get_timer_factory()
                      ->create_interval_timer(std::chrono::seconds(1));
    stats_timer->start([this] {
      auto all = core->get_session_module()->get_peers()->get_all();
      for (auto &do_ : all)
        do_->rtc_connection().get_stats();
    });
  }

private:
  int run() override {
    auto joiner = core->get_session_module()->get_joiner();
    client::joiner::parameters parameters{uuid::generate(), "fun"};
    joiner->join(parameters).then(executor, [this](auto result) {
      try {
        result.get();
      } catch (...) {
        run_promise.set_exception(boost::current_exception());
        stop_execution();
      }
    });
    core->get_utils_executor_module()->get_io_context()->run();
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
  rtc::google::log_webrtc_to_logging log_webrtc_to_logging;
  boost::inline_executor executor;
  std::shared_ptr<client::core_module> core;
  std::shared_ptr<client::audio_module> audio;
  std::shared_ptr<client::audio_settings_module> audio_settings;
  std::shared_ptr<utils::interval_timer> stats_timer;
  boost::promise<int> run_promise;
};
} // namespace

std::unique_ptr<audio_client::audio_client>
audio_client::audio_client::create(const config &config_) {
  return std::make_unique<audio_client_impl>(config_);
}
