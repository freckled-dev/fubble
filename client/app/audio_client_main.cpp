#include "audio_client.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/log_webrtc_to_logging.hpp"

int main(int, char *[]) {
  logging::add_console_log(logging::severity::debug);
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(false);
  logging::logger logger{"main"};
  audio_client::audio_client::config audio_client_config;
  auto audio_client = audio_client::audio_client::create(audio_client_config);
  return audio_client->run();
}
