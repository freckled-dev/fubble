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

void client::client::close() {
  BOOST_ASSERT(connection_);
  connection_->close();
}

void client::client::operator()(const std::string &host,
                                const std::string &service,
                                const std::string &key) {
  websocket::connector::config connector_config;
  connector_config.url = host;
  connector_config.service = service;
  connector(connector_config).then(executor, [this, key](auto result) {
    connected(result, key);
  });
}

void client::client::connected(boost::future<websocket::connection_ptr> &result,
                               const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
  try {
    auto websocket_connection = result.get();
    connection_ptr connection_ = connection_creator_(websocket_connection);
    this->connection_ = connection_;
    connect_signals(connection_);
    on_connected();
    connection_->send_registration(signalling::registration{key});
    connection_->run().then(
        [this](boost::future<void> result) { run_done(result); });
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

void client::client::connect_signals(const connection_ptr &connection_) const {
  connection_->on_create_offer.connect([this] { on_create_offer(); });
}

void client::client::run_done(boost::future<void> &result) {
  logging::logger logger;
  try {
    result.get();
    BOOST_LOG_SEV(logger, logging::severity::info) << "connection stopped";
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "connection stopped running with error:" << error.what();
  } catch (...) {
    BOOST_ASSERT(false);
  }
}
