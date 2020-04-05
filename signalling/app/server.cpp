#include "signalling/server/server.hpp"
#include "boost_di_extension_scopes_session.hpp"
#include "executor_asio.hpp"
#include "logging/initialser.hpp"
#include "signalling/device/creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/registration_handler.hpp"
#include "signalling/server/connection_creator.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/di.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <iostream>

namespace {
struct options {
  int port{};
};
} // namespace

static std::optional<options> parse_options(int argc, char *argv[]);
static void set_up_logging();

int main(int argc, char *argv[]) {
  auto options_optional = parse_options(argc, argv);
  if (!options_optional)
    return 0;
  auto options = options_optional.value();
  set_up_logging();
  logging::logger logger{"main"};
  websocket::acceptor::config acceptor_config{
      static_cast<std::uint16_t>(options.port)};
  boost::asio::io_context context;
  using executor_type = boost::executor_adaptor<executor_asio>;
  auto injector = boost::di::make_injector<boost::di::extension::shared_config>(
      boost::di::bind<boost::asio::io_context>.to(context),
      boost::di::bind<websocket::acceptor::config>.to(acceptor_config),
      boost::di::bind<boost::executor>.to(
          std::make_shared<executor_type>(context)));
  auto &server = injector.create<signalling::server::server &>();
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("server is listening on port {}", server.port());

  boost::asio::signal_set signals{context, SIGINT, SIGTERM};
  signals.async_wait([&server](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    server.close();
  });
  context.run();

  return 0;
}

static void set_up_logging() { logging::add_console_log(); }

static std::optional<options> parse_options(int argc, char *argv[]) {
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
