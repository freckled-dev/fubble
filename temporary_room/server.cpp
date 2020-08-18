#include "logging/initialser.hpp"
#include "temporary_room/server/application.hpp"
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace {
struct options {
  unsigned short port{80};
  std::string matrix_server{"localhost"};
  std::string matrix_port{"8008"};
  std::string matrix_target_prefix{"/_matrix/client/r0/"};
}; // namespace

std::optional<options> parse_options(int argc, char *argv[]) {
  namespace bpo = boost::program_options;
  options result;
  bpo::options_description general("Allowed options");
  general.add_options()("help", "produce help message");
  general.add_options()(
      "port,p",
      bpo::value<unsigned short>(&result.port)->default_value(result.port),
      "port server will listen on");
  general.add_options()("matrix-server",
                        bpo::value<std::string>(&result.matrix_server)
                            ->default_value(result.matrix_server),
                        "matrix server to connect to");
  general.add_options()("matrix-port",
                        bpo::value<std::string>(&result.matrix_port)
                            ->default_value(result.matrix_port),
                        "matrix server port to connect to");
  general.add_options()("matrix-target-prefix",
                        bpo::value<std::string>(&result.matrix_target_prefix)
                            ->default_value(result.matrix_target_prefix),
                        "matrix server target prefix to use");
  bpo::variables_map variables_map;
  bpo::store(bpo::parse_command_line(argc, argv, general), variables_map);
  bpo::notify(variables_map);
  if (variables_map.count("help") > 0) {
    std::cout << general << std::endl;
    return {};
  }
  return result;
}

void set_up_logging() { logging::add_console_log(logging::severity::debug); }
} // namespace

int main(int argc, char *argv[]) {
  using namespace temporary_room;
  auto options_check = parse_options(argc, argv);
  if (!options_check)
    return 1;
  auto options_ = options_check.value();
  set_up_logging();
  logging::logger logger{"main_logger"};
  boost::asio::io_context context;

  server::application::options options_casted;
  options_casted.matrix_port = options_.matrix_port;
  options_casted.matrix_server = options_.matrix_server;
  options_casted.matrix_target_prefix = options_.matrix_target_prefix;
  options_casted.port = options_.port;
  auto application = server::application::create(context, options_casted);
  auto application_result = application->run();

  boost::asio::signal_set signals{context, SIGINT, SIGTERM};
  signals.async_wait([&application](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    application->close();
  });
  BOOST_LOG_SEV(logger, logging::severity::debug) << "context.run()";
  context.run();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "after context.run()";
  auto check_for_operation_cancelled = [](boost::future<void> &check) {
    try {
      check.get();
    } catch (const boost::system::system_error &error) {
      if (error.code() == boost::asio::error::operation_aborted)
        return;
      std::rethrow_exception(std::current_exception());
    }
  };
  check_for_operation_cancelled(application_result);
  return 0;
}
