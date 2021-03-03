#include <boost/asio/io_context.hpp>
#include <fubble/app/cli_client.hpp>
#include <fubble/client/log_module.hpp>
#include <fubble/utils/exit_signals.hpp>
#include <fubble/utils/logging/logger.hpp>

int main(int, char *[]) {
  client::log_module::config log_config;
#if 1
  log_config.webrtc = true;
  log_config.severity = logging::debug;
#endif
  client::log_module log_module_{log_config};
  logging::logger logger{"main"};
  fubble::cli_client::config config;
  config.send_audio = false;
  config.use_v4l2_hw_h264 = true;
  auto &session_config = config.core.session_;
  session_config.receive_audio = session_config.receive_video = false;
  auto client = fubble::cli_client::create(config);
  exit_signals exit_signals_{client->get_core()
                                 ->get_utils_executor_module()
                                 ->get_io_context()
                                 ->get_executor()};
  exit_signals_.async_wait([&](auto error) {
    BOOST_ASSERT(!error);
    if (error)
      return;
    client->stop();
  });
  return client->run();
}
