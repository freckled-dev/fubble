#include "fubble/utils/logging/initialser.hpp"
#include "fubble/utils/options_adder.hpp"
#include "fubble/version/server.hpp"
#include <boost/asio/signal_set.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  logging::add_console_log(logging::severity::debug);
  logging::logger logger{"main_logger"};
  BOOST_LOG_SEV(logger, logging::severity::info) << "starting up";
  version::server::config config;
  config.port = 8085;
  {
    namespace bpo = boost::program_options;
    bpo::options_description general("general");
    general.add_options()("help", "produce help message");

    bpo::options_description server("server");
    {
      utils::option_adder adder{server};
      adder.add("address", config.address, "address to bind to");
      adder.add("port", config.port, "port to bind to");
      adder.add("current_version", config.current_version,
                "the latest version that's released");
      adder.add("minimum_version", config.minimum_version,
                "the minimum_version version that's needed to be compatible");
    }

    bpo::options_description options;
    options.add(general);
    options.add(server);
    bpo::variables_map vm;
    bpo::command_line_parser command_line_parser{argc, argv};
    bpo::store(command_line_parser.options(options).run(), vm);
    bpo::notify(vm);
    if (vm.count("help")) {
      std::cout << options << std::endl;
      return {};
    }
  }
  std::shared_ptr<boost::asio::io_context> context =
      std::make_shared<boost::asio::io_context>();
  auto server = version::server::create(context, config);

  boost::asio::signal_set signals{*context, SIGINT, SIGTERM};
  signals.async_wait([&](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    BOOST_LOG_SEV(logger, logging::severity::info) << "shutting down";
    server.reset();
  });
  context->run();
  return 0;
}
