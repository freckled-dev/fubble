#include "connector.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/asio/connect.hpp>
#include <fmt/format.h>

using namespace websocket;

struct connector::state {
  boost::promise<connection_ptr> promise;
  connector::config config_;
};

connector::connector(boost::asio::io_context &context,
                     connection_creator &creator)
    : resolver(context), creator(creator) {}

boost::future<connection_ptr> connector::operator()(const config &config_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "connecting to host: '{}', service:'{}'", config_.url, config_.service);
  auto state_ = std::make_shared<state>();
  state_->config_ = config_;
  auto result = state_->promise.get_future();
  resolve(state_, config_);
  return result;
}

void connector::resolve(state_ptr &state_, const config &config_) {
  resolver.async_resolve(
      config_.url, config_.service,
      [state_, this](const auto &error, const auto &endpoints) mutable {
        if (error) {
          state_->promise.set_exception(boost::system::system_error(error));
          return;
        }
        connect_to_endpoints(state_, endpoints);
      });
}

void connector::connect_to_endpoints(
    state_ptr &state_,
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  auto connection_ = creator();
  auto &native = connection_->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  boost::asio::async_connect(
      tcp, endpoints,
      [this, state_, connection_](const auto &error, const auto &) mutable {
        if (error) {
          state_->promise.set_exception(boost::system::system_error(error));
          return;
        }
        handshake(state_, connection_);
      });
}

void connector::handshake(state_ptr &state_, connection_ptr connection_) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "goiing to handshake";
  auto &native = connection_->get_native();
  native.async_handshake(
      state_->config_.url, state_->config_.path,
      [this, connection_, state_](const auto &error) mutable {
        BOOST_LOG_SEV(logger, logging::severity::trace) << "did handshake";
        if (error) {
          state_->promise.set_exception(boost::system::system_error(error));
          return;
        }
        BOOST_LOG_SEV(logger, logging::severity::info)
            << "did handshake successfully";
        state_->promise.set_value(connection_);
      });
}
