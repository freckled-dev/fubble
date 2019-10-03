#include "server.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

server::server::server(boost::asio::io_context &context,
                       websocket::acceptor &acceptor,
                       connection_creator &connection_creator_)
    : context(context), acceptor(acceptor),
      connection_creator_(connection_creator_) {}

void server::server::run() {
  boost::asio::spawn(context, [this](auto yield) { run(yield); });
}

void server::server::run(boost::asio::yield_context yield) {
  try {
    while (true) {
      auto websocket_connection = acceptor(yield);
      BOOST_ASSERT(websocket_connection);
      boost::asio::spawn(context,
                         [this, connection = std::move(websocket_connection)](
                             boost::asio::yield_context yield) {
                           handle_connection(*connection, yield);
                         });
    }
  } catch (boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "got the error: '{}'. Will stop accepting new connections",
        error.what());
  }
}

void server::server::close() {}

void server::server::handle_connection(
    websocket::connection &websocket_connection,
    boost::asio::yield_context yield) {
  try {
    auto connection_ = connection_creator_(websocket_connection);
    handle_connection(*connection_, yield);
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << fmt::format("connection threw an error, message:'{}'", error.what());
  }
}

void server::server::handle_connection(connection &connection_,
                                       boost::asio::yield_context yield) {
  connection_.run(yield);
}
