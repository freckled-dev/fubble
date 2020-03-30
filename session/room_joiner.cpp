#include "room_joiner.hpp"
#include "client.hpp"
#include "room.hpp"
#include <fmt/format.h>

using namespace session;

room_joiner::room_joiner(client &client_) : client_(client_) {}

boost::future<room_joiner::room_ptr>
room_joiner::join(const std::string &room_) {
  (void)room_;
  room_ptr result;
  return boost::make_ready_future(std::move(result));
}

