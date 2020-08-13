#include "add_data_channel.hpp"
#include "rtc/connection.hpp"

using namespace client;

add_data_channel::~add_data_channel() = default;

void add_data_channel::add_to_connection(rtc::connection &connection) {
  auto channel = connection.create_data_channel();
  BOOST_ASSERT(channel);
  on_added(channel);
}

void add_data_channel::remove_from_connection([
    [maybe_unused]] rtc::connection &connection) {
  // BOOST_ASSERT(false && "implement");
  // TODO save channel per connection and remove it here
}
