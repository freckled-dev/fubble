#include "room_joiner.hpp"
#include "client.hpp"
#include "room.hpp"
#include <fmt/format.h>

using namespace session;

room_joiner::room_joiner(client &client_) : client_(client_) {}

boost::future<room_joiner::room_ptr>
room_joiner::join(const std::string &room_) {
  auto result = promise.get_future();
  auto native = client_.get_native_realtime_client();
  auto persistance = Nakama::opt::nullopt;
  auto hidden = Nakama::opt::nullopt;
  native->joinChat(
      room_, Nakama::NChannelType::ROOM, persistance, hidden,
      [self = shared_from_this()](auto session) { self->on_success(session); },
      [self = shared_from_this()](auto error) { self->on_error(error); });
  return result;
}

void room_joiner::on_success(Nakama::NChannelPtr channel) {
  auto result = std::make_unique<room>(client_, channel);
  promise.set_value(std::move(result));
}

void room_joiner::on_error(Nakama::NRtError error) {
  promise.set_exception(std::runtime_error(
      fmt::format("failed to join room, message:'{}'", error.message)));
}
