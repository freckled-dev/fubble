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
                           on_resolved(error, endpoints);
                         });
}

void connector::on_resolved(
    const boost::system::error_code &error,
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_resolved, error:" << error.message()
      << ", results.size():" << endpoints.size();
  if (check_error(error))
    return;
  connect_to_endpoints(endpoints);
}

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>; // not needed as of C++20

void connector::connect_to_endpoints(
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  connection = creator.create(config_.ssl);
  auto &native = connection->get_native();
  boost::asio::ip::tcp::socket *tcp =
      std::visit(overloaded{[&](connection::http_stream_type &stream_) {
                              return &stream_.next_layer();
                            },
                            [&](connection::https_stream_type &stream_) {
                              return &stream_.next_layer().next_layer();
                            }},
                 native);
  boost::asio::async_connect(
      *tcp, endpoints,
      [this](const auto &error, const auto &) { on_connected(error); });
}

void connector::on_connected(const boost::system::error_code &error) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_connected, error:" << error.message();
  if (check_error(error))
    return;
  if (!config_.ssl)
    return handshake();
  secure();
}

void connector::secure() {
  // TODO unify with http::connection_cretor::secure!!!
  auto &stream =
      std::get<connection::https_stream_type>(connection->get_native());
  auto &ssl_context = connection->get_ssl_context();
  ssl_context.set_default_verify_paths();
  ssl_context.set_verify_mode(boost::asio::ssl::verify_none);
  // TODO replace the following line with certify
  if (!SSL_set_tlsext_host_name(stream.next_layer().native_handle(),
                                config_.url.c_str())) {
    boost::beast::error_code error{static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category()};
    check_error(error);
    return;
  }
  stream.next_layer().async_handshake(
      boost::asio::ssl::stream_base::client,
      [this](const auto error) { on_secured(error); });
}

void connector::on_secured(const boost::system::error_code &error) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_secured, error:" << error.message();
  if (check_error(error))
    return;
  handshake();
}

void connector::handshake() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "goiing to handshake";
  auto &native = connection->get_native();
  std::visit(
      [&](auto &item) {
        item.async_handshake(
            config_.url, config_.path, [this](const auto &error) {
              BOOST_LOG_SEV(this->logger, logging::severity::trace)
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
