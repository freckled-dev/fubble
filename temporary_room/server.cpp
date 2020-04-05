#include "server/server.hpp"
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
  bpo::variables_map variables_map;
  bpo::store(bpo::parse_command_line(argc, argv, general), variables_map);
  bpo::notify(variables_map);
  if (variables_map.count("help") > 0) {
    std::cout << general << std::endl;
    return {};
  }
  return result;
}

void set_up_logging() { logging::add_console_log(); }
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
  http_fields_matrix.target_prefix = "/_matrix/client/r0/";
  http::client_factory http_client_factory_matrix{context, http_server_matrix,
                                                  http_fields_matrix};
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
  signals.async_wait([&acceptor](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    acceptor.stop();
  });
  BOOST_LOG_SEV(logger, logging::severity::trace) << "context.run()";
  context.run();
  BOOST_LOG_SEV(logger, logging::severity::trace) << "after context.run()";
  acceptor_done.get();
  return 0;
}
