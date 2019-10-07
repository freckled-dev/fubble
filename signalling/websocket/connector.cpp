#include "connector.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/asio/connect.hpp>
#include <fmt/format.h>

using namespace websocket;

connector::connector(boost::asio::io_context &context,
                     connection_creator &creator)
    : resolver(context), creator(creator) {}

std::unique_ptr<connection> connector::
operator()(const config &config_, boost::asio::yield_context yield) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "connecting to host: '{}', service:'{}'", config_.url, config_.service);
  auto endpoints = resolver.async_resolve(config_.url, config_.service, yield);
  auto result = creator();
  auto &native = result->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  boost::asio::async_connect(tcp, endpoints, yield);
  native.async_handshake(config_.url, config_.path, yield);
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "connected to host: '{}', service:'{}'", config_.url, config_.service);
  return result;
}
