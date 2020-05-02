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
  }
  bpo::options_description options;
  options.add(general);
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);
  if (vm.count("help")) {
    std::cout << options << std::endl;
    return {};
  }
  return result;
}
