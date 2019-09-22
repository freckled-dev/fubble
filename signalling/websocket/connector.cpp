#include "connector.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/asio/connect.hpp>

using namespace websocket;

connector::connector(boost::asio::io_context &context,
                     connection_creator &creator, const config &config_)
    : resolver(context), creator(creator), config_(config_) {}

std::unique_ptr<connection> connector::
operator()(boost::asio::yield_context yield) {
  auto endpoints = resolver.async_resolve(config_.url, config_.service, yield);
  auto result = creator();
  auto &native = result->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  boost::asio::async_connect(tcp, endpoints, yield);
  native.async_handshake(config_.url, config_.path, yield);
  return result;
}

