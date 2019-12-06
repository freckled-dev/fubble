#include "options.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace bpo = boost::program_options;

namespace {
struct option_adder {
  bpo::options_description &options;
  template <class value_type>
  void operator()(std::string flag, value_type &value,
                  std::string description) {
    options.add_options()(flag.c_str(),
                          bpo::value<value_type>(&value)->default_value(value),
                          description.c_str());
  }
};
} // namespace

std::optional<config> options::operator()(int argc, char *argv[]) {
  config result;

  bpo::options_description general("general");
  general.add_options()("help", "produce help message");

  bpo::options_description signalling("signalling");
  option_adder adder{signalling};
  adder("signalling-host", result.signalling_.host,
        "hostname of the signalling server");
  adder("signalling-service", result.signalling_.service,
        "service of signalling server. eg \"http\" or 80");
  adder("signalling-id", result.signalling_.id,
        "the id where two peers shall meet");

  bpo::options_description options;
  options.add(general);
  options.add(signalling);

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return {};
  }

  return result;
}
