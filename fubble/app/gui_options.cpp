#include "gui_options.hpp"
#include "fubble/utils/options_adder.hpp"
#include <iostream>

namespace bpo = boost::program_options;

boost::optional<gui_config> gui_options::parse(int argc, char *argv[]) {
  gui_config result;
  bpo::options_description general("general");
  general.add_options()("help", "produce help message");

  {
    utils::option_adder adder{general};
    adder.add("host", result.general_.host, "hostname of the server");
    adder.add("service", result.general_.service,
              "service of the server. eg \"http\" or 80");
    adder.add("use-ipv6", result.general_.use_ipv6,
              "enable or disable the usage of ipv6 in webrtc");
    adder.add("use-ssl", result.general_.use_ssl,
              "enable or disable the usage of ssl");
    adder.add("use-crash-catcher", result.general_.use_crash_catcher,
              "enable or disable the usage of the stacktrace crash catcher");
    adder.add(
        "log-webrtc", result.general_.log_webrtc,
        "enable or disable the logging of webrtc, which is quite verbose");
    adder.add("video", result.general_.video_support,
              "enable or disable the usage of video-support");
    adder.add("log-severity", result.general_.log_severity,
              "the must-have log_severity for being handled. valid values are: "
              "[trace, debug, info, warning, error, fatal]");
    adder.add("audio-layer", result.client_core.rtc_.audio_.layer_,
              "choose the audio-layer to use. eg. linux_alsa or linux_pulse");
  }
  bpo::options_description options;
  options.add(general);
  bpo::variables_map vm;
  // allow unregistered options
  // https://stackoverflow.com/questions/15552284/boostprogram-options-how-to-ignore-unknown-parameters
  bpo::command_line_parser command_line_parser{argc, argv};
  bpo::store(command_line_parser.options(options).allow_unregistered().run(),
             vm);
  bpo::notify(vm);
  if (vm.count("help")) {
    std::cout << options << std::endl;
    return {};
  }
  return result;
}
