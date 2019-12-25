#include "client.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include "websocket/connector.hpp"
#include <fmt/format.h>
#include <iostream>

using namespace signalling;

client::client::client(boost::executor &executor_,
                       websocket::connector &connector_,
                       connection_creator &connection_creator_)
    : executor(executor_), connector(connector_),
      connection_creator_(connection_creator_) {}

void client::client::set_connect_information(const connect_information &set) {
  connect_information_ = set;
}

void client::client::close() {
  if (!connection_)
    return;
  connection_->close();
}

void client::client::send_offer(const signalling::offer &offer_) {
  connection_->send_offer(offer_);
}

void client::client::send_answer(const signalling::answer &answer_) {
  connection_->send_answer(answer_);
}

void client::client::send_ice_candidate(
    const signalling::ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

void client::client::connect(const std::string &key) {
  BOOST_ASSERT(!connection_);
  websocket::connector::config connector_config;
  connector_config.url = connect_information_.host;
  connector_config.service = connect_information_.service;
  connector(connector_config).then(executor, [this, key](auto result) {
    connected(result, key);
  });
}

void client::client::connected(boost::future<websocket::connection_ptr> &result,
                               const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
  try {
    auto websocket_connection = result.get();
    BOOST_ASSERT(!connection_);
    connection_ = connection_creator_(websocket_connection);
    connect_signals(connection_);
    on_connected();
    connection_->send_registration(signalling::registration{key});
    connection_->run().then(
        executor, [this](boost::future<void> result) { run_done(result); });
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
  connection_->on_create_answer.connect([this] { on_create_answer(); });
  connection_->on_offer.connect(
      [this](const auto &offer_) { on_offer(offer_); });
  connection_->on_answer.connect(
      [this](const auto &answer_) { on_answer(answer_); });
  connection_->on_ice_candidate.connect(
      [this](const auto &candidate) { on_ice_candidate(candidate); });
}

void client::client::run_done(boost::future<void> &result) {
  logging::logger logger;
  try {
    result.get();
    BOOST_LOG_SEV(logger, logging::severity::info) << "connection stopped";
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "connection stopped running with error:" << error.what();
    on_error(error);
  } catch (...) {
    BOOST_ASSERT(false);
  }
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "stopped reading from connection";
  on_closed();
}
