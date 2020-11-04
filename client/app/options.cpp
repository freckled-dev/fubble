#include "options.hpp"
#include "utils/options_adder.hpp"
#include <iostream>

namespace bpo = boost::program_options;

namespace {
auto video(config::video &result) {
  bpo::options_description description("video");
  utils::option_adder adder{description};
  adder.add("video-send", result.send, "send a video");
  return description;
}
} // namespace

boost::optional<config> options::operator()(int argc, char *argv[]) {
  config result;

  bpo::options_description general("general");
  general.add_options()("help", "produce help message");

  auto video_ = video(result.video_);
  bpo::options_description signaling("signaling");
  {
    utils::option_adder adder{signaling};
    adder.add("signaling-host", result.signaling_.host,
              "hostname of the signaling server");
    adder.add("signaling-service", result.signaling_.service,
              "service of signaling server. eg \"http\" or 80");
    adder.add("signaling-id", result.signaling_.id,
              "the id where two peers shall meet");
  }

#if 0
  bpo::options_description matrix("matrix");
  {
    utils::option_adder adder{matrix};
    adder.add("matrix-host", result.matrix_.host, "hostname of the matrix server");
    adder.add("matrix-service", result.matrix_.service,
          "service of matrix server. eg \"http\" or 80");
  }

  bpo::options_description temporary_room("temporary_room");
  {
    utils::option_adder adder{temporary_room};
    adder.add("temporary_room-host", result.temporary_room_.host,
          "hostname of the temporary_room server");
    adder.add("temporary_room-service", result.temporary_room_.service,
          "service of temporary_room server. eg \"http\" or 80");
  }
#endif

  bpo::options_description options;
  options.add(general);
  options.add(video_);
  options.add(signaling);
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
