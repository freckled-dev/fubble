#include "server.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include <boost/thread/executors/executor.hpp>
#include <fmt/format.h>
#include <signalling/registration_handler.hpp>

server::server::server(boost::executor &executor_,
                       websocket::acceptor &acceptor,
                       connection_creator &connection_creator_,
                       signalling::registration_handler &registration_handler)
    : executor(executor_), acceptor(acceptor),
      connection_creator_(connection_creator_),
      registration_handler(registration_handler) {
  acceptor.on_connection.connect(
      [this](auto connection_) { on_connection(connection_); });
}

server::server::~server() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~server()";
}

void server::server::on_connection(
    websocket::connection_ptr websocket_connection) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "got a server connection";
  connection_ptr connection_ = connection_creator_(websocket_connection);
  registration_handler.add(connection_);
  connection_->run().then(executor, [connection_](auto result) {});
}

void server::server::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing server";
  acceptor.close();
}
