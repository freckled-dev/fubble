#include "connector.hpp"
#include "connection_creator.hpp"
#include "connection_impl.hpp"
#include "fubble/http/ssl_upgrader.hpp"
#include <boost/asio/connect.hpp>
#include <fmt/format.h>

using namespace websocket;

connector::connector(boost::asio::io_context &context,
                     connection_creator &creator, const config &config_)
    : creator(creator), resolver{context}, config_(config_) {}

connector::~connector() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "~connector()";
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
                           on_resolved(error, endpoints);
                         });
}

void connector::on_resolved(
    const boost::system::error_code &error,
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", results.size():" << endpoints.size();
  if (check_error(error)) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << ", error:" << error.message();
    return;
  }
  connect_to_endpoints(endpoints);
}

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>; // not needed as of C++20

void connector::connect_to_endpoints(
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  connection = creator.create(config_.ssl);
  auto connection_impl_ = dynamic_cast<connection_impl *>(connection.get());
  BOOST_ASSERT(connection_impl_);
  auto &native = connection_impl_->get_native();
  auto *tcp = boost::apply_visitor(
      overloaded{[&](connection_impl::http_stream_type &stream_) {
                   return &stream_.next_layer();
                 },
                 [&](connection_impl::https_stream_type &stream_) {
                   return &stream_.next_layer().next_layer();
                 }},
      native);
  boost::asio::async_connect(
      *tcp, endpoints,
      [this](const auto &error, const auto &) { on_connected(error); });
}

void connector::on_connected(const boost::system::error_code &error) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (check_error(error)) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << ", error:" << error.message();
    return;
  }
  if (!config_.ssl)
    return handshake();
  secure();
}

void connector::secure() {
  // TODO unify with http::connection_cretor::secure!!!
  auto connection_impl_ = dynamic_cast<connection_impl *>(connection.get());
  BOOST_ASSERT(connection_impl_);
  auto &stream = boost::get<connection_impl::https_stream_type>(
      connection_impl_->get_native());
  auto &ssl_context = connection_impl_->get_ssl_context();
  http::server server_;
  server_.host = config_.url;
  server_.port = config_.service;
  auto upgrader =
      std::make_shared<http::ssl_upgrader<connection_impl::ssl_stream_type>>(
          server_, stream.next_layer(), ssl_context);
  upgrader->secure_connection().then([this, upgrader](auto result) {
    try {
      result.get();
      on_secured();
    } catch (const boost::system::system_error &error) {
      check_error(error.code());
    }
  });
}

void connector::on_secured() { handshake(); }

void connector::handshake() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "goiing to handshake";
  auto connection_impl_ = dynamic_cast<connection_impl *>(connection.get());
  BOOST_ASSERT(connection_impl_);
  auto &native = connection_impl_->get_native();
  boost::apply_visitor(
      [&](auto &item) {
        item.async_handshake(
            config_.url, config_.path, [this](const auto &error) {
              BOOST_LOG_SEV(this->logger, logging::severity::debug)
                  << "did handshake";
              if (check_error(error))
                return;
              BOOST_LOG_SEV(this->logger, logging::severity::info)
                  << "did handshake successfully";
              done = true;
              promise.set_value(std::move(connection));
            });
      },
      native);
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
