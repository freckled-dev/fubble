#include "client.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>
#include <iostream>

client::client::client(boost::asio::io_context &context,
                       websocket::connector &connector,
                       connection_creator &connection_creator_)
    : context(context), connector(connector),
      connection_creator_(connection_creator_) {}

boost::future<client::connection_ptr> client::client::
operator()(const std::string &host, const std::string &service) {
  boost::promise<connection_ptr> promise;
  auto result = promise.get_future();
  websocket::connector::config connector_config;
  connector_config.url = host;
  connector_config.service = service;
  boost::asio::spawn(context, [this, connector_config,
                               promise_ =
                                   std::move(promise)](auto yield) mutable {
    try {
      connection_ptr result = connect(connector_config, yield);
      connection_ = result;
      promise_.set_value(result);
    } catch (const boost::system::system_error &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << fmt::format("got an error connecting, error: '{}'", error.what());
      promise_.set_exception(error);
    }
  });
  return result;
}

client::connection_ptr
client::client::connect(const websocket::connector::config &config,
                        boost::asio::yield_context yield) {
  websocket::connection_ptr websocket_connection = connector(config, yield);
  return connection_creator_(websocket_connection);
}
