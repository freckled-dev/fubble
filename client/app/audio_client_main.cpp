#include "audio_client.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/log_webrtc_to_logging.hpp"
#include "utils/exit_signals.hpp"
#include <boost/asio/io_context.hpp>

int main(int, char *[]) {
  logging::add_console_log(logging::severity::debug);
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(false);
  logging::logger logger{"main"};
  audio_client::audio_client::config audio_client_config;
  auto audio_client = audio_client::audio_client::create(audio_client_config);
  boost::asio::executor exit_executor = audio_client->get_core()
                                            ->get_utils_executor_module()
                                            ->get_io_context()
                                            ->get_executor();
  exit_signals exit_signals_{exit_executor};
  exit_signals_.async_wait([&](auto) { audio_client->stop(); });
  auto result = audio_client->run();
  return result.get();
}
