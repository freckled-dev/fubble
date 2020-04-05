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
auto video(config::video &result) {
  bpo::options_description description("video");
  option_adder adder{description};
  adder("video-send", result.send, "send a video");
  return description;
}
} // namespace

std::optional<config> options::operator()(int argc, char *argv[]) {
  config result;

  bpo::options_description general("general");
  general.add_options()("help", "produce help message");

  auto video_ = video(result.video_);
  bpo::options_description signalling("signalling");
  {
    option_adder adder{signalling};
    adder("signalling-host", result.signalling_.host,
          "hostname of the signalling server");
    adder("signalling-service", result.signalling_.service,
          "service of signalling server. eg \"http\" or 80");
    adder("signalling-id", result.signalling_.id,
          "the id where two peers shall meet");
  }

#if 0
  bpo::options_description matrix("matrix");
  {
    option_adder adder{matrix};
    adder("matrix-host", result.matrix_.host, "hostname of the matrix server");
    adder("matrix-service", result.matrix_.service,
          "service of matrix server. eg \"http\" or 80");
  }

  bpo::options_description temporary_room("temporary_room");
  {
    option_adder adder{temporary_room};
    adder("temporary_room-host", result.temporary_room_.host,
          "hostname of the temporary_room server");
    adder("temporary_room-service", result.temporary_room_.service,
          "service of temporary_room server. eg \"http\" or 80");
  }
#endif

  bpo::options_description options;
  options.add(general);
  options.add(video_);
  options.add(signalling);
#if 0
  options.add(matrix);
  options.add(temporary_room);
#endif

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return {};
  }

  return result;
}
