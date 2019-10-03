#include "server.hpp"
#include "server/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include <gtest/gtest.h>
#include <websocket/connection_creator.hpp>

TEST(Server, SetUp) {
  boost::asio::io_context context;
  websocket::acceptor::config acceptor_config;
  websocket::connection_creator websocket_connection_creator(context);
  websocket::acceptor acceptor(context, websocket_connection_creator,
                               acceptor_config);
  signalling::json_message json_message;
  server::connection_creator server_connection_creator(json_message);
  server::server server_(context, acceptor, server_connection_creator);
}

