#include "connection_creator.hpp"
#include "connector.hpp"

using namespace http;

connection_creator::connection_creator(boost::asio::io_context &context)
    : context(context) {}

std::unique_ptr<connection> connection_creator::create(const server &server_) {
  auto tcp_connection = std::make_unique<http_connection>(context);
  if (!server_.secure)
    return tcp_connection;
  auto ssl_connection =
      std::make_unique<https_connection>(std::move(tcp_connection));
  return ssl_connection;
}

std::unique_ptr<connector>
connection_creator::create_connector(connection &connection_,
                                     const server &server_) {
  return std::make_unique<connector>(context, connection_, server_);
}
