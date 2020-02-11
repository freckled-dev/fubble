#include "room_joiner.hpp"
#include "client.hpp"
#include "room.hpp"
#include <fmt/format.h>

using namespace session;

room_joiner::room_joiner(client &client_) : client_(client_) {}

boost::future<room_joiner::room_ptr>
room_joiner::join(const std::string &room_) {
  auto result = promise.get_future();
  auto &natives = client_.get_natives();
  auto persistance = Nakama::opt::nullopt;
  auto hidden = Nakama::opt::nullopt;
  natives.realtime_client->joinChat(
      room_, Nakama::NChannelType::ROOM, persistance, hidden,
      [this](auto session) { on_success(session); },
      [this](auto error) { on_error(error); });
  return result;
}

void room_joiner::on_success(Nakama::NChannelPtr channel) {
  auto result = std::make_unique<room>(client_, channel);
  promise.set_value(std::move(result));
}

void room_joiner::on_error(Nakama::NRtError error) {
  auto error_message =
      fmt::format("failed to join room, message:'{}'", error.message);
  BOOST_LOG_SEV(logger, logging::severity::warning) << error_message;
  promise.set_exception(std::runtime_error(error_message));
}
