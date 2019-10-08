#include "client.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>
#include <iostream>

client::client::client(boost::executor &executor_,
                       websocket::connector &connector_,
                       connection_creator &connection_creator_)
    : executor(executor_), connector(connector_),
      connection_creator_(connection_creator_) {}

void client::client::operator()(const std::string &host,
                                const std::string &service) {
  websocket::connector::config connector_config;
  connector_config.url = host;
  connector_config.service = service;
  connector(connector_config).then(executor, [this](auto result) {
    connected(result);
  });
}

void client::client::connected(
    boost::future<websocket::connection_ptr> &result) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
  try {
    auto websocket_connection = result.get();
    connection_ptr connection_ = connection_creator_(websocket_connection);
    this->connection_ = connection_;
    on_connected();
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "an error occured, what:" << error.what();
    on_error(error);
  } catch (...) {
    BOOST_ASSERT(false);
  }
}

client::connection_ptr client::client::get_connection() const {
  return connection_;
}
