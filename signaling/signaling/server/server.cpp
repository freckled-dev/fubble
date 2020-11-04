#include "server.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection.hpp"
#include <boost/thread/executors/executor.hpp>
#include <fmt/format.h>
#include <signaling/registration_handler.hpp>

using namespace signaling::server;
using clazz = signaling::server::server;

clazz::server(boost::executor &executor_, websocket::acceptor &acceptor,
              connection_creator &connection_creator_,
              signaling::registration_handler &registration_handler)
    : executor(executor_), acceptor(acceptor),
      connection_creator_(connection_creator_),
      registration_handler(registration_handler) {
  acceptor.on_connection.connect(
      [this](auto &connection_) { on_connection(std::move(connection_)); });
}

clazz::~server() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "~server()";
}

void clazz::on_connection(websocket::connection_ptr websocket_connection) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "got a server connection";
  connection_ptr connection_ =
      connection_creator_.create(std::move(websocket_connection));
  registration_handler.add(connection_);
  connection_->run().then(executor, [connection_](auto) {});
}

void clazz::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing server";
  acceptor.close();
}

int clazz::get_port() const { return acceptor.get_port(); }
