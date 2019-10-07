#include "server.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

server::server::server(boost::asio::io_context &context,
                       websocket::acceptor &acceptor,
                       connection_creator &connection_creator_)
    : context(context), acceptor(acceptor),
      connection_creator_(connection_creator_) {
  run();
}

void server::server::run() {
  boost::asio::spawn(context, [this](auto yield) { run(yield); });
}

void server::server::run(boost::asio::yield_context yield) {
  try {
    while (true) {
      websocket::connection_ptr websocket_connection = acceptor(yield);
      BOOST_ASSERT(websocket_connection);
      boost::asio::spawn(context, [this, websocket_connection](
                                      boost::asio::yield_context yield) {
        handle_connection(websocket_connection, yield);
      });
    }
  } catch (boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "got the error: '{}'. Will stop accepting new connections",
        error.what());
  }
}

void server::server::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing server";
  acceptor.close();
}

void server::server::handle_connection(
    const websocket::connection_ptr &websocket_connection,
    boost::asio::yield_context yield) {
  try {
    connection_ptr connection_ = connection_creator_(websocket_connection);
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
