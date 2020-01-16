#include "room.hpp"
#include "client.hpp"
#include <fmt/format.h>

using namespace session;

room::room(client &client_, Nakama::NChannelPtr channel)
    : client_(client_), channel(channel) {
  on_nakama_joins(channel->presences);
  auto &client_signals = client_.get_natives().realtime_signals;
  signal_connections.push_back(client_signals.on_channel_presence.connect(
      [this](const auto &event) { on_channel_presence(event); }));
  signal_connections.push_back(client_signals.on_channel_message.connect(
      [this](const auto &event) { on_channel_message(event); }));
}

const room::participants &room::get_participants() const {
  return participants_;
}

const std::string &room::get_name() const { return channel->roomName; }

const std::string &room::own_id() const { return channel->self.userId; }

void room::on_channel_message(const Nakama::NChannelMessage &message) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_channel_message, content:" << message.content;
}

void room::on_channel_presence(const Nakama::NChannelPresenceEvent &event) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("on_channel_presence, joins:{}, leaves:{}",
                     event.joins.size(), event.leaves.size());
  on_nakama_joins(event.joins);
}

namespace {
participant
convert_nakama_presence_to_participant(const Nakama::NUserPresence &convert) {
  participant result;
  result.id = convert.userId;
  return result;
}
} // namespace

void room::on_nakama_joins(
    const std::vector<Nakama::NUserPresence> &presences) {
  if (presences.empty())
    return;
  {
    participants joins;
    std::transform(presences.cbegin(), presences.cend(),
                   std::back_inserter(joins),
                   convert_nakama_presence_to_participant);
    std::copy(joins.cbegin(), joins.cend(), std::back_inserter(participants_));
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << fmt::format("self:{} joining_id:{}", own_id(), joins.back().id);
    on_joins(joins);
  }
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("there are {} participants", participants_.size());
  {
    Nakama::NClientPtr native_client = client_.get_natives().client_;
    Nakama::NSessionPtr native_session = client_.get_natives().session_;
    std::vector<std::string> ids;
    for (const auto presence : presences)
      ids.push_back(presence.userId);
    std::vector<std::string> usernames;
    std::vector<std::string> facebook_ids;
    native_client->getUsers(
        native_session, ids, usernames, facebook_ids,
        [this](const auto users) { on_names(users); },
        [this](auto error) { on_error(error); });
  }
}

void room::on_names(const Nakama::NUsers &users) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_names, users.size():" << users.users.size();
  std::vector<participant> updates;
  for (auto &user : users.users) {
    auto found =
        std::find_if(participants_.begin(), participants_.end(),
                     [&](const auto &item) { return item.id == user.id; });
    if (found == participants_.end()) {
      BOOST_LOG_SEV(logger, logging::severity::error) << fmt::format(
          "could not find id participant in list, id:{}", user.id);
      BOOST_ASSERT(false);
      continue;
    }
    found->name = user.displayName;
    updates.push_back(*found);
  }
  BOOST_ASSERT(!updates.empty());
  if (updates.empty())
    return;
  on_updates(updates);
}

void room::on_error(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::error)
      << "on_error, message:" << error.message;
}
