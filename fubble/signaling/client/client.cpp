#include "client.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "fubble/utils/timer.hpp"
#include "fubble/websocket/connection.hpp"
#include "fubble/websocket/connector.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <fmt/format.h>
#include <iostream>
#include <queue>

using namespace signaling::client;

namespace {

class client_impl : public client {
public:
  client_impl(websocket::connector_creator &connector_,
              connection_creator &connection_creator_parameter)
      : connector_creator(connector_),
        connection_creator_(connection_creator_parameter) {}

  ~client_impl() = default;

  void set_connect_information(const connect_information &set) override {
    connect_information_ = set;
  }

  boost::future<void> close() override {
    if (!connection_)
      return boost::make_exceptional(
          boost::system::system_error(boost::asio::error::not_connected));
    return connection_->close();
  }

  void send_offer(const signaling::offer &offer_) override {
    BOOST_ASSERT(connection_);
    BOOST_LOG_SEV(logger, logging::severity::debug) << "send_offer";
    connection_->send_offer(offer_);
  }

  void send_answer(const signaling::answer &answer_) override {
    BOOST_ASSERT(connection_);
    BOOST_LOG_SEV(logger, logging::severity::debug) << "send_answer";
    connection_->send_answer(answer_);
  }

  void send_ice_candidate(const signaling::ice_candidate &candidate) override {
    BOOST_ASSERT(connection_);
    BOOST_LOG_SEV(logger, logging::severity::debug) << "send_ice_candidate";
    connection_->send_ice_candidate(candidate);
  }

  void send_want_to_negotiate() override {
    BOOST_ASSERT(connection_);
    BOOST_LOG_SEV(logger, logging::severity::debug) << "send_want_to_negotiate";
    connection_->send_want_to_negotiate();
  }

  void connect(const std::string &token, const std::string &key) override {
    BOOST_ASSERT(!connection_);
    websocket::connector::config connector_config;
    connector_config.ssl = connect_information_.secure;
    connector_config.url = connect_information_.host;
    connector_config.service = connect_information_.service;
    connector_config.path = connect_information_.target;
    BOOST_ASSERT(!connector);
    connector = connector_creator.create(connector_config);
    connector->connect().then(executor, [this, key, token](auto result) {
      connected(result, token, key);
    });
  }

protected:
  void connected(boost::future<websocket::connection_ptr> &result,
                 const std::string &token, const std::string &key) {
    BOOST_LOG_SEV(logger, logging::severity::info) << "client connected";
    try {
      auto websocket_connection = result.get();
      BOOST_ASSERT(!connection_);
      connection_ = connection_creator_.create(std::move(websocket_connection));
      connect_signals(connection_);
      connection_->send_registration(signaling::registration{key, token});
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

  void connect_signals(const connection_ptr &connection_) {
    connection_->on_create_offer.connect([this] { on_create_offer(); });
    connection_->on_offer.connect(
        [this](const auto &offer_) { on_offer(offer_); });
    connection_->on_answer.connect(
        [this](const auto &answer_) { on_answer(answer_); });
    connection_->on_ice_candidate.connect(
        [this](const auto &candidate) { on_ice_candidate(candidate); });
  }

  void run_done(boost::future<void> &result) {
    signaling::logger logger{"client::run_done"};
    try {
      result.get();
      BOOST_LOG_SEV(logger, logging::severity::info) << "connection stopped";
    } catch (const boost::system::system_error &error) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "connection stopped running with error:" << error.what();
      if (error.code() != boost::asio::error::operation_aborted)
        on_error(error);
    } catch (const boost::broken_promise &error) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "a broken_promise should not happen error occured";
      BOOST_ASSERT(false);
    } catch (...) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "run_done, an unknown error occured";
      BOOST_ASSERT(false);
    }
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "stopped reading from connection";
    on_closed();
  }
  signaling::logger logger{"client"};
  boost::inline_executor executor;
  websocket::connector_creator &connector_creator;
  connection_creator &connection_creator_;
  connect_information connect_information_;
  std::unique_ptr<websocket::connector> connector;
  connection_ptr connection_;
};

class reconnecting_client : public client {
public:
  reconnecting_client(factory &factory_, utils::one_shot_timer &timer)
      : factory_(factory_), timer(timer) {}

  ~reconnecting_client() {
    signal_connections.clear();
    timer.stop();
  }

  void set_connect_information(const connect_information &set) override {
    information = set;
    if (!delegate)
      return;
    delegate->set_connect_information(set);
  }

  void connect(const std::string &token_, const std::string &key_) override {
    BOOST_ASSERT(!delegate);
    BOOST_ASSERT(key.empty());
    token = token_;
    key = key_;
    reconnect();
  }

  boost::future<void> close() override {
    if (!delegate) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", no delegate!";
      BOOST_ASSERT(false);
      return boost::make_ready_future();
    }
    BOOST_ASSERT(shall_reconnect);
    shall_reconnect = false;
    return delegate->close();
  }

  void send_offer(const signaling::offer &offer_) override {
    if (is_connected())
      return delegate->send_offer(offer_);
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << "not connected";
    BOOST_ASSERT(!offer_cache);
    offer_cache = offer_;
  }

  void send_answer(const signaling::answer &answer_) override {
    if (is_connected())
      return delegate->send_answer(answer_);
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << "not connected";
    BOOST_ASSERT(!answer_cache);
    answer_cache = answer_;
  }

  void send_ice_candidate(const signaling::ice_candidate &candidate_) override {
    if (is_connected())
      return delegate->send_ice_candidate(candidate_);
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << "not connected";
    candidates_cache.push_back(candidate_);
  }

  void send_want_to_negotiate() override {
    if (is_connected())
      return delegate->send_want_to_negotiate();
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << "not connected";
    BOOST_ASSERT(!wants_to_negotiate_cache);
    wants_to_negotiate_cache = true;
  }

protected:
  bool is_connected() const { return delegate != nullptr; }

  void got_registered() {
    // connected
    if (!is_registered) {
      is_registered = true;
      on_registered();
    }
    send_cache();
  }

  void reconnect() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    if (!shall_reconnect) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "wont reconnect due to !shall_reconnect";
      return;
    }
    delegate = factory_.create();
    BOOST_ASSERT(signal_connections.empty());
    signal_connections.push_back(delegate->on_answer.connect(
        [this](const auto &answer_) { on_answer(answer_); }));
    signal_connections.push_back(delegate->on_offer.connect(
        [this](const auto &offer_) { on_offer(offer_); }));
    signal_connections.push_back(
        delegate->on_ice_candidate.connect([this](const auto &ice_candidate_) {
          on_ice_candidate(ice_candidate_);
        }));
    signal_connections.push_back(delegate->on_error.connect(
        [this](const auto &error_) { got_error(error_); }));
    signal_connections.push_back(
        delegate->on_create_offer.connect([this] { on_create_offer(); }));
    signal_connections.push_back(
        delegate->on_registered.connect([this] { got_registered(); }));
    signal_connections.push_back(
        delegate->on_closed.connect([this] { got_closed(); }));
    delegate->connect(token, key);
  }

  void got_error(const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << ", error:" << error.what();
    // no need to do anything do to the signal `on_closed` will get called
  }

  void got_closed() {
    BOOST_LOG_SEV(logger, logging::severity::warning) << __FUNCTION__;
    reconnect_after_timeout();
  }

  void reconnect_after_timeout() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    if (!shall_reconnect) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "wont reconnect due to !shall_reconnect";
      return;
    }
    delegate.reset();
    signal_connections.clear();
    timer.start([this] { reconnect(); });
  }

  void send_cache() {
    for (const auto &candidate : candidates_cache)
      send_ice_candidate(candidate);
    candidates_cache.clear();
    if (offer_cache)
      send_offer(offer_cache.value());
    offer_cache.reset();
    if (answer_cache)
      send_answer(answer_cache.value());
    answer_cache.reset();
    if (wants_to_negotiate_cache)
      send_want_to_negotiate();
    wants_to_negotiate_cache = false;
  }

  signaling::logger logger{"reconnecting_client"};
  std::vector<sigslot::scoped_connection> signal_connections;
  factory &factory_;
  utils::one_shot_timer &timer;
  std::unique_ptr<client> delegate;
  connect_information information;
  std::string token;
  std::string key;
  bool is_registered{};
  bool shall_reconnect{true};

  bool wants_to_negotiate_cache{};
  boost::optional<signaling::answer> answer_cache;
  boost::optional<signaling::offer> offer_cache;
  std::vector<signaling::ice_candidate> candidates_cache;
};

} // namespace

std::unique_ptr<client>
client::create(websocket::connector_creator &connector_creator,
               connection_creator &connection_creator_) {
  return std::make_unique<client_impl>(connector_creator, connection_creator_);
}

std::unique_ptr<client>
client::create_reconnecting(factory &factory_, utils::one_shot_timer &timer) {
  return std::make_unique<reconnecting_client>(factory_, timer);
}

factory_reconnecting::factory_reconnecting(factory &factory_,
                                           utils::one_shot_timer &timer)
    : factory_(factory_), timer(timer) {}

std::unique_ptr<client> factory_reconnecting::create() {
  return client::create_reconnecting(factory_, timer);
}

factory_impl::factory_impl(websocket::connector_creator &connector_creator,
                           connection_creator &connection_creator_,
                           const connect_information &connect_information_)
    : connector_creator(connector_creator),
      connection_creator_(connection_creator_),
      connect_information_(connect_information_) {}

std::unique_ptr<client> factory_impl::create() {
  auto result = client::create(connector_creator, connection_creator_);
  result->set_connect_information(connect_information_);
  return result;
}
