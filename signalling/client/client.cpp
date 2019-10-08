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

boost::future<client::connection_ptr> client::client::
operator()(const std::string &host, const std::string &service) {
  websocket::connector::config connector_config;
  connector_config.url = host;
  connector_config.service = service;
  boost::future<connection_ptr> result =
      connector(connector_config).then(executor, [this](auto result) {
        BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
        auto websocket_connection = result.get();
        connection_ptr connection_ = connection_creator_(websocket_connection);
        return connection_;
      });
  return result;
}
