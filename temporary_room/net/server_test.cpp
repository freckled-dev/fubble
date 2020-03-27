#include "server.hpp"
#include <gtest/gtest.h>

TEST(NetServer, Instance) {
  boost::asio::io_context context;
  temporary_room::net::server::server::config config;
  temporary_room::net::server::server server{context, config};
}
