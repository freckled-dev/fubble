#include "room_joiner.hpp"
#include "client.hpp"
#include "http/client.hpp"
#include "room.hpp"
#include <fmt/format.h>

using namespace session;

room_joiner::room_joiner(temporary_room::net::client &temporary_room_client)
    : temporary_room_client{temporary_room_client} {}

room_joiner::~room_joiner() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~room_joiner";
}

boost::future<room_joiner::room_ptr>
room_joiner::join(client &client_, const std::string &room_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "join room:" << room_;
  return temporary_room_client.join(room_, client_.get_natives().get_user_id())
      .then(executor,
            [this, &client_](auto result) {
              return on_got_invited(client_, result);
            })
      .unwrap()
      .then(executor, [this](auto result) { return on_joined(result); });
}

boost::future<matrix::room *>
room_joiner::on_got_invited(client &client_,
                            boost::future<std::string> &result) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_got_invited";
  auto room_id = result.get();
  return client_.get_natives().get_rooms().join_room_by_id(room_id);
}

room_joiner::room_ptr
room_joiner::on_joined(boost::future<matrix::room *> &result) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_joined";
  matrix::room *room_ = result.get();
  return std::make_unique<room>(*room_);
}
