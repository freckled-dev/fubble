#include "server.hpp"
#include <gtest/gtest.h>

TEST(NetServer, Instance) {
  boost::asio::io_context context;
  temporary_room::net::server::acceptor::config config;
  temporary_room::net::server::acceptor server{context, config};
}
