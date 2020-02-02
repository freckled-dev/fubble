#include "connector.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/asio/connect.hpp>
#include <fmt/format.h>

using namespace websocket;

connector::connector(boost::asio::io_context &context,
                     connection_creator &creator, const config &config_)
    : creator(creator), resolver{context}, config_(config_) {}

connector::~connector() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~connector()";
  if (done)
    return;
  check_error(boost::asio::error::operation_aborted);
}

boost::future<connection_ptr> connector::connect() {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "connecting to host: '{}', service:'{}'", config_.url, config_.service);

  auto result = promise.get_future();
  resolve();
  return result;
}

void connector::resolve() {
  resolver.async_resolve(config_.url, config_.service,
                         [this](const auto &error, const auto &endpoints) {
                           if (check_error(error))
                             return;
                           connect_to_endpoints(endpoints);
                         });
}

void connector::connect_to_endpoints(
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  connection = creator();
  auto &native = connection->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  boost::asio::async_connect(tcp, endpoints,
                             [this](const auto &error, const auto &) mutable {
                               if (check_error(error))
                                 return;
                               handshake();
                             });
}

void connector::handshake() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "goiing to handshake";
  auto &native = connection->get_native();
  native.async_handshake(config_.url, config_.path, [this](const auto &error) {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "did handshake";
    if (check_error(error))
      return;
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "did handshake successfully";
    done = true;
    promise.set_value(std::move(connection));
  });
}

bool connector::check_error(const boost::system::error_code &error) {
  if (!error)
    return false;
  done = true;
  promise.set_exception(boost::system::system_error(error));
  return true;
}

connector_creator::connector_creator(boost::asio::io_context &context,
                                     connection_creator &creator)
    : context(context), creator(creator) {}

connector_creator::~connector_creator() = default;

std::unique_ptr<connector>
connector_creator::create(const connector::config &config_) {
  return std::make_unique<connector>(context, creator, config_);
}
