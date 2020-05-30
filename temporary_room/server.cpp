#include "server/server.hpp"
#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "logging/initialser.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "server/matrix_rooms_factory_adapter.hpp"
#include "server/server.hpp"
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

void set_up_logging() { logging::add_console_log(logging::severity::trace); }
} // namespace

int main(int argc, char *argv[]) {
  using namespace temporary_room;
  auto options_check = parse_options(argc, argv);
  if (!options_check)
    return 1;
  auto options_ = options_check.value();
  set_up_logging();
  logging::logger logger{"main_logger"};
  boost::inline_executor executor;
  boost::asio::io_context context;
  // matrix
  matrix::factory matrix_factory;
  http::server http_server_matrix{options_.matrix_server, options_.matrix_port};
  http::fields http_fields_matrix{http_server_matrix};
  http_fields_matrix.target_prefix = options_.matrix_target_prefix;
  http::connection_creator connection_creator_{context};
  http::action_factory action_factory_{connection_creator_};
  http::client_factory http_client_factory_matrix{
      action_factory_, http_server_matrix, http_fields_matrix};
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory_matrix};
  matrix::authentification matrix_authentification{http_client_factory_matrix,
                                                   matrix_client_factory};
  // matrix client for temporary_room::server
  auto matrix_client_server_future =
      matrix_authentification.register_anonymously();
  context.run();
  context.reset();
  auto matrix_client_server = matrix_client_server_future.get();
  auto matrix_client_server_sync_result =
      matrix_client_server->sync_till_stop();
  matrix_client_server->set_display_name("Fubble Bot");
  // temporary_room::server
  net::server::acceptor::config acceptor_config;
  acceptor_config.port = options_.port;
  net::server::acceptor acceptor{context, acceptor_config};
  net::server::server net_server{acceptor};
  server::matrix_rooms_factory_adapter rooms_factory{*matrix_client_server};
  rooms::rooms rooms{rooms_factory};
  server::server server{net_server, rooms};
  // lets start to accept connections
  auto acceptor_done = acceptor.run();
  boost::asio::signal_set signals{context, SIGINT, SIGTERM};
  signals.async_wait(
      [&acceptor, &matrix_client_server](const auto &error, auto) {
        if (error == boost::asio::error::operation_aborted)
          return;
        acceptor.stop();
        matrix_client_server->stop_sync();
      });
  BOOST_LOG_SEV(logger, logging::severity::trace) << "context.run()";
  context.run();
  BOOST_LOG_SEV(logger, logging::severity::trace) << "after context.run()";
  auto check_for_operation_cancelled = [](boost::future<void> &check) {
    try {
      check.get();
    } catch (const boost::system::system_error &error) {
      if (error.code() == boost::asio::error::operation_aborted)
        return;
      std::rethrow_exception(std::current_exception());
    }
  };
  check_for_operation_cancelled(matrix_client_server_sync_result);
  check_for_operation_cancelled(acceptor_done);
  return 0;
}
