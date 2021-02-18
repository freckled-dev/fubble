#include "fubble/utils/logging/logger.hpp"
#include <fubble/app/cli_client.hpp>
#include <fubble/client/log_module.hpp>

int main(int, char *[]) {
  client::log_module::config log_config;
  client::log_module log_module_{log_config};
  logging::logger logger{"main"};
  fubble::cli_client::config config;
  config.send_audio = false;
  auto &session_config = config.core.session_;
  session_config.receive_audio = session_config.receive_video = false;
  auto client = fubble::cli_client::create(config);
  return client->run();
}
