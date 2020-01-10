#include "room.hpp"
#include "client.hpp"
#include "fmt/format.h"

using namespace session;

room::room(client &client_, Nakama::NChannelPtr channel)
    : client_(client_), channel(channel) {
  on_nakama_joins(channel->presences);
  client_.on_channel_presence.connect(
      [this](const auto &event) { on_channel_presence(event); });
  client_.on_channel_message.connect(
      [this](const auto &event) { on_channel_message(event); });
}

const std::string &room::get_name() const { return channel->roomName; }

const std::string &room::own_id() const { return channel->self.userId; }

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
  {
    participants joins;
    std::transform(presences.cbegin(), presences.cend(),
                   std::back_inserter(joins),
                   convert_nakama_presence_to_participant);
    std::copy(joins.cbegin(), joins.cend(), std::back_inserter(participants_));
    on_joins(joins);
  }
  {
    Nakama::NClientPtr native_client = client_.get_native_client();
    Nakama::NSessionPtr native_session = client_.get_native_session();
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
