#include "fubble/utils/logging/initialser.hpp"
#include "options.hpp"

int main(int argc, char *argv[]) {
  logging::add_console_log(logging::severity::debug);
  logging::logger logger{"main"};
  BOOST_LOG_SEV(logger, logging::severity::info) << "starting";

  options options_parser;
  auto parsed_config = options_parser(argc, argv);
  if (!parsed_config)
    return 0;
  const auto config_ = parsed_config.value();
  // TODO
  return 0;
}
