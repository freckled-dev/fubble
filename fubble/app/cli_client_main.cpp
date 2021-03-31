#include "options.hpp"
#include <boost/asio/io_context.hpp>
#include <fubble/app/cli_client.hpp>
#include <fubble/client/log_module.hpp>
#include <fubble/utils/exit_signals.hpp>
#include <fubble/utils/logging/logger.hpp>
#include <fubble/utils/options_adder.hpp>
#include <iostream>

namespace bpo = boost::program_options;

bool parse_options(int argc, char *argv[],
                   client::log_module::config &log_config,
                   fubble::cli_client::config &config) {
  bpo::options_description options;
  bpo::options_description general("general");
  general.add_options()("help", "produce help message");
  utils::option_adder adder{general};
  adder.add("send-audio", config.send_audio, "send audio");
  adder.add("send-video", config.send_video, "send video");
  adder.add("use-v4l2-hw-h264", config.use_v4l2_hw_h264,
            "use the v4l2 library with hardware h264 support. Works well with "
            "the USB cameras that got h264 on board and the raspberry pi.");
  options.add(general);
  options.add(fubble::client::options::create(log_config));
  options.add(fubble::client::options::create(config.v4l2_hw_h264_config));
  bpo::command_line_parser command_line_parser{argc, argv};
  bpo::variables_map vm;
  bpo::store(command_line_parser.options(options).run(), vm);
  bpo::notify(vm);
  if (vm.count("help") == 0)
    return true;
  std::cout << options << std::endl;
  return false;
}

int main(int argc, char *argv[]) {
  client::log_module::config log_config;
  fubble::cli_client::config config;
  if (!parse_options(argc, argv, log_config, config))
    return 1;
  client::log_module log_module_{log_config};
  logging::logger logger{"main"};
  auto &session_config = config.core.session_;
  session_config.receive_audio = session_config.receive_video = false;
  config.video.video_enumerator_refresh_timeout =
      std::chrono::minutes(1); // check for new video devices every minute, an
                               // not for every "few seconds".
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
