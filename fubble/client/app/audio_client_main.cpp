#include "audio_client.hpp"
#include "fubble/utils/logging/initialser.hpp"
#include "fubble/utils/logging/logger.hpp"
#include "rtc/google/log_webrtc_to_logging.hpp"
#include "utils/exit_signals.hpp"
#include "utils/options_adder.hpp"
#include <boost/asio/io_context.hpp>
#include <iostream>

namespace bpo = boost::program_options;

int main(int argc, char *argv[]) {
  audio_client::audio_client::config audio_client_config;

  bpo::options_description general("general");
  general.add_options()("help", "produce help message");
  utils::option_adder adder{general};
  adder.add("send-audio", audio_client_config.send_audio,
            "send your own audio stream");
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, general), vm);
  bpo::notify(vm);

  if (vm.count("help")) {
    std::cout << general << std::endl;
    return 0;
  }

  logging::add_console_log(logging::severity::debug);
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(false);
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
