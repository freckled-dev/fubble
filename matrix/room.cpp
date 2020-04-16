#include "room.hpp"
#include "client.hpp"
#include "error.hpp"
#include <fmt/format.h>

using namespace matrix;

room::room(client &client_, const std::string &id) : client_(client_), id(id) {
  BOOST_ASSERT(!id.empty());
  http_client = client_.create_http_client();
}

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

const room::members_type &room::get_members() const { return members; }

std::optional<user *> room::get_member_by_id(const std::string &id) {
  auto found =
      std::find_if(members.begin(), members.end(),
                   [&](const auto &check) { return check->get_id() == id; });
  if (found == members.end())
    return {};
  return *found;
}

std::string room::get_name() const { return name; }

void room::sync(const nlohmann::json &content) {
  const auto joined_rooms = content["rooms"]["join"];
  BOOST_ASSERT(joined_rooms.contains(id));
  if (!joined_rooms.contains(id))
    return;
  auto our_room = joined_rooms[id];
  auto events = our_room["timeline"]["events"];
  for (const auto &event : events) {
    const std::string type = event["type"];
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
  if (membership == "leave")
    return remove_member(user_id);
  if (membership == "join") {
    member member_;
    member_.display_name = content["displayname"];
    member_.id = user_id;
    return add_or_update_member(member_);
  }
  // lots more - not implemented
}

void room::on_event_m_room_name(const nlohmann::json &parse) {
  name = parse["content"]["name"];
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "room_name changed to:" << name;
  on_name_changed(name);
}

void room::add_or_update_member(const member &member_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("add_member, id:'{}', display_name:'{}'", member_.id,
                     member_.display_name);
  user &add_or_update = client_.get_users().get_or_add_user(member_.id);
  add_or_update.set_display_name(member_.display_name);
  if (get_member_by_id(member_.id))
    return;
  members.push_back(&add_or_update);
  on_join(add_or_update);
}

void room::remove_member(const std::string &user_id) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("remove_member, id:'{}'", user_id);
  const auto found =
      std::find_if(members.cbegin(), members.cend(),
                   [&](auto &check) { return check->get_id() == user_id; });
  if (found == members.cend()) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "cant remove user because not found, user_id:" << user_id
        << ", this may occur if user left before we joined";
    return;
  }
  members.erase(found);
  on_leave(user_id);
}
