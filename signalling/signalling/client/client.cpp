#include "client.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "websocket/connection.hpp"
#include "websocket/connector.hpp"
#include <fmt/format.h>
#include <iostream>

using namespace signalling;

client::client::client(websocket::connector_creator &connector_,
                       connection_creator &connection_creator_parameter)
    : connector_creator(connector_),
      connection_creator_(connection_creator_parameter) {}

client::client::~client() = default;

void client::client::set_connect_information(const connect_information &set) {
  connect_information_ = set;
}

boost::future<void> client::client::close() {
  if (!connection_)
    return boost::make_exceptional(
        boost::system::system_error(boost::asio::error::not_connected));
  return connection_->close();
}

void client::client::send_offer(const signalling::offer &offer_) {
  BOOST_ASSERT(connection_);
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_offer";
  connection_->send_offer(offer_);
}

void client::client::send_answer(const signalling::answer &answer_) {
  BOOST_ASSERT(connection_);
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_answer";
  connection_->send_answer(answer_);
}

void client::client::send_ice_candidate(
    const signalling::ice_candidate &candidate) {
  BOOST_ASSERT(connection_);
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_ice_candidate";
  connection_->send_ice_candidate(candidate);
}

void client::client::send_want_to_negotiate() {
  BOOST_ASSERT(connection_);
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_want_to_negotiate";
  connection_->send_want_to_negotiate();
}

void client::client::connect(const std::string &key) {
  BOOST_ASSERT(!connection_);
  websocket::connector::config connector_config;
  connector_config.ssl = connect_information_.secure;
  connector_config.url = connect_information_.host;
  connector_config.service = connect_information_.service;
  connector_config.path = connect_information_.target;
  BOOST_ASSERT(!connector);
  connector = connector_creator.create(connector_config);
  connector->connect().then(
      executor, [this, key](auto result) { connected(result, key); });
}

void client::client::connected(boost::future<websocket::connection_ptr> &result,
                               const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
  try {
    auto websocket_connection = result.get();
    BOOST_ASSERT(!connection_);
    connection_ = connection_creator_.create(std::move(websocket_connection));
    connect_signals(connection_);
    connection_->send_registration(signalling::registration{key});
    on_registered();
    connection_->run().then(
        executor, [this](boost::future<void> result) { run_done(result); });
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "an asio error occured, what:" << error.what();
    on_error(error);
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "an error occured, what:" << error.what();
    BOOST_ASSERT(false);
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "an unknow error occured";
    BOOST_ASSERT(false);
  }
}

client::connection &client::client::get_connection() const {
  return *connection_;
}

void client::client::connect_signals(const connection_ptr &connection_) const {
  connection_->on_create_offer.connect([this] { on_create_offer(); });
  connection_->on_offer.connect(
      [this](const auto &offer_) { on_offer(offer_); });
  connection_->on_answer.connect(
      [this](const auto &answer_) { on_answer(answer_); });
  connection_->on_ice_candidate.connect(
      [this](const auto &candidate) { on_ice_candidate(candidate); });
}

void client::client::run_done(boost::future<void> &result) {
  signalling::logger logger{"client::run_done"};
  try {
    result.get();
    BOOST_LOG_SEV(logger, logging::severity::info) << "connection stopped";
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "connection stopped running with error:" << error.what();
    on_error(error);
  } catch (const boost::broken_promise &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "a broken_promise should not happen error occured";
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "run_done, an unknown error occured";
    BOOST_ASSERT(false);
  }
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "stopped reading from connection";
  on_closed();
}
