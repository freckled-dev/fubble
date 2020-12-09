#include "fubble/signaling/server/server.hpp"
#include "fubble/signaling/server/application.hpp"
#include "fubble/utils/logging/initialser.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <iostream>

namespace {
struct options {
  int port{};
};
} // namespace

static boost::optional<options> parse_options(int argc, char *argv[]);
static void set_up_logging();

int main(int argc, char *argv[]) {
  auto options_optional = parse_options(argc, argv);
  if (!options_optional)
    return 0;
  auto options = options_optional.value();
  set_up_logging();
  logging::logger logger{"main"};
  boost::asio::io_context context;
  auto server = signaling::server::application::create(context, options.port);
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("server is listening on port {}", server->get_port());

  boost::asio::signal_set signals{context, SIGINT, SIGTERM};
  signals.async_wait([&server](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    server->close();
  });
  context.run();

  return 0;
}

static void set_up_logging() {
  logging::add_console_log(logging::severity::debug);
}

static boost::optional<options> parse_options(int argc, char *argv[]) {
  namespace bpo = boost::program_options;
  options result;
  bpo::options_description general("Allowed options");
  general.add_options()("help", "produce help message");
  general.add_options()(
      "port,p", bpo::value<int>(&result.port)->default_value(result.port),
      "port server will listen on");
  bpo::variables_map variables_map;
  bpo::store(bpo::parse_command_line(argc, argv, general), variables_map);
  bpo::notify(variables_map);
  if (variables_map.count("help") > 0) {
    std::cout << general << std::endl;
    return {};
  }
  return result;
}
