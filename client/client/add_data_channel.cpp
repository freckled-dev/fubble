#include "add_data_channel.hpp"
#include "rtc/connection.hpp"

using namespace client;

add_data_channel::~add_data_channel() = default;

void add_data_channel::add_to_connection(rtc::connection &connection) {
  auto channel = connection.create_data_channel();
  BOOST_ASSERT(channel);
  on_added(channel);
}
