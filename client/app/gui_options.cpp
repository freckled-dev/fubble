#include "gui_options.hpp"
#include "options_adder.hpp"
#include <iostream>

namespace bpo = boost::program_options;

std::optional<gui_config> gui_options::parse(int argc, char *argv[]) {
  gui_config result;
  bpo::options_description general("general");
  general.add_options()("help", "produce help message");

  {
    option_adder adder{general};
    adder.add("host", result.general_.host, "hostname of the server");
    adder.add("service", result.general_.service,
              "service of the server. eg \"http\" or 80");
    adder.add("use-ipv6", result.general_.use_ipv6,
              "enable or disable the usage of ipv6 in webrtc");
    adder.add("use-ssl", result.general_.use_ssl,
              "enable or disable the usage of ssl");
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
