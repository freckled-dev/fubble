#include "audio_client.hpp"
#include "fubble/rtc/google/log_webrtc_to_logging.hpp"
#include "fubble/utils/exit_signals.hpp"
#include "fubble/utils/logging/initialser.hpp"
#include "fubble/utils/logging/logger.hpp"
#include "fubble/utils/options_adder.hpp"
#include <boost/asio/io_context.hpp>
#include <iostream>

namespace bpo = boost::program_options;

int main(int argc, char *argv[]) {
  audio_client::audio_client::config audio_client_config;
  auto &core = audio_client_config.core;

  bpo::options_description general("general");
  general.add_options()("help", "produce help message");
  {
    utils::option_adder adder{general};
    adder.add("send-audio", audio_client_config.send_audio,
              "send your own audio stream");
    adder.add("audio-layer", audio_client_config.core.rtc_.audio_layer_,
              "choose the audio-layer to use. eg. linux_alsa or linux_pulse");
  }
  bpo::options_description signaling("signaling");
  {
    utils::option_adder adder{signaling};
    adder.add("signaling-host", core.signaling_.host, "domain or ip");
    adder.add("signaling-use-ssl", core.signaling_.secure, "use ssl");
    adder.add("signaling-service", core.signaling_.service,
              "port or service. like \"80\" or \"http\"");
    adder.add("signaling-target", core.signaling_.target,
              "target prefix. like \"api/v0\"");
  }
  bpo::options_description matrix("matrix");
  {
    utils::option_adder adder{matrix};
    adder.add("matrix-host", core.matrix_.host, "domain or ip");
    adder.add("matrix-use-ssl", core.matrix_.use_ssl, "use ssl");
    adder.add("matrix-service", core.matrix_.service,
              "port or service. like \"80\" or \"http\"");
    adder.add("matrix-target", core.matrix_.target,
              "target prefix. like \"api/v0\"");
  }
  bpo::options_description temporary_room("temporary_room");
  {
    utils::option_adder adder{temporary_room};
    adder.add("temporary_room-host", core.temporary_room_.host, "domain or ip");
    adder.add("temporary_room-use-ssl", core.temporary_room_.use_ssl,
              "use ssl");
    adder.add("temporary_room-service", core.temporary_room_.service,
              "port or service. like \"80\" or \"http\"");
    adder.add("temporary_room-target", core.temporary_room_.target,
              "target prefix. like \"api/v0\"");
  }
  bpo::options_description version("version");
  {
    utils::option_adder adder{version};
    adder.add("version-host", core.version_.host, "domain or ip");
    adder.add("version-use-ssl", core.version_.use_ssl, "use ssl");
    adder.add("version-service", core.version_.service,
              "port or service. like \"80\" or \"http\"");
    adder.add("version-target", core.version_.target,
              "target prefix. like \"api/v0\"");
  }
  bpo::options_description options;
  options.add(general);
  options.add(signaling);
  options.add(matrix);
  options.add(temporary_room);
  options.add(version);
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }

  logging::add_console_log(logging::severity::debug);
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(false);
  logging::logger logger{"main"};
  auto audio_client = audio_client::audio_client::create(audio_client_config);
  audio_client->set_stats_callback([&logger](std::string out) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << out;
  });
  boost::asio::executor exit_executor = audio_client->get_core()
                                            ->get_utils_executor_module()
                                            ->get_io_context()
                                            ->get_executor();
  exit_signals exit_signals_{exit_executor};
  exit_signals_.async_wait([&](auto) { audio_client->stop(); });
  auto result = audio_client->run();
  return result;
}
