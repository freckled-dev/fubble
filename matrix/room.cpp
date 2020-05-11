#include "room.hpp"
#include "chat.hpp"
#include "client.hpp"
#include "error.hpp"
#include "room_participant.hpp"
#include <fmt/format.h>

using namespace matrix;

room::room(client &client_, const std::string &id)
    : client_(client_), id(id), chat_{std::make_unique<chat>(client_, id)} {
  BOOST_ASSERT(!id.empty());
  http_client = client_.create_http_client();
}

room::~room() = default;

const std::string &room::get_id() const { return id; }

boost::future<void> room::invite_by_user_id(const std::string &user_id) {
  BOOST_ASSERT(!user_id.empty());
  auto target = fmt::format("rooms/{}/invite", id);
  auto request = nlohmann::json::object();
  request["user_id"] = user_id;
  return http_client->post(target, request).then(executor, [](auto result) {
    error::check_matrix_response(result);
  });
}

boost::future<void> room::kick(const std::string &user_id) {
  BOOST_ASSERT(!user_id.empty());
  auto target = fmt::format("rooms/{}/kick", id);
  auto request = nlohmann::json::object();
  request["user_id"] = user_id;
  return http_client->post(target, request).then(executor, [](auto result) {
    error::check_matrix_response(result);
  });
}

std::vector<room_participant *> room::get_members() const {
  std::vector<room_participant *> result;
  std::transform(members.cbegin(), members.cend(), std::back_inserter(result),
                 [](auto &item) { return item.get(); });
  return result;
}

std::optional<room_participant *>
room::get_member_by_id(const std::string &id) {
  auto found =
      std::find_if(members.begin(), members.end(),
                   [&](const auto &check) { return check->get_id() == id; });
  if (found == members.end())
    return {};
  return found->get();
}

std::string room::get_name() const { return name; }

chat &room::get_chat() const { return *chat_; }

void room::sync(const nlohmann::json &content) {
  const auto joined_rooms = content["rooms"]["join"];
  BOOST_ASSERT(joined_rooms.contains(id));
  if (!joined_rooms.contains(id))
    return;
  auto our_room = joined_rooms[id];
  auto events = our_room["timeline"]["events"];
  for (const auto &event : events) {
    const std::string type = event["type"];
    if (chat_->sync_event(type, event))
      continue;
    if (type == "m.room.member") {
      on_event_m_room_member(event);
      continue;
    }
    if (type == "m.room.name") {
      BOOST_LOG_SEV(logger, logging::severity::trace) << "m.room.name";
      on_event_m_room_name(event);
      continue;
    }
  }
}

void room::on_event_m_room_member(const nlohmann::json &parse) {
  // https://matrix.org/docs/spec/client_server/latest#m-room-member
  // Adjusts the membership state for a user in a room.
  const std::string user_id = parse["state_key"];
  const auto content = parse["content"];
  const std::string membership = content["membership"];
  const std::optional<std::string> display_name =
      [&]() -> std::optional<std::string> {
    auto found = content.find("displayname");
    if (found != content.cend())
      return *found;
    return std::nullopt;
  }();
  const join_state join_state_ = [&] {
    if (membership == "leave")
      return join_state::leave;
    if (membership == "join")
      return join_state::join;
    if (membership == "invite")
      return join_state::invite;
    BOOST_ASSERT(false);
    return join_state::join;
  }();
  user &add_or_update = client_.get_users().get_or_add_user(user_id);
  if (display_name)
    add_or_update.set_display_name(display_name.value());

  auto got = get_member_by_id(user_id);
  if (got)
    return got.value()->set_join_state(join_state_);

  auto add = std::make_unique<room_participant>(add_or_update, join_state_);
  members.push_back(std::move(add));
  on_join(*members.back());
}

void room::on_event_m_room_name(const nlohmann::json &parse) {
  name = parse["content"]["name"];
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "room_name changed to:" << name;
  on_name_changed(name);
}

