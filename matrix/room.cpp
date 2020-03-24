#include "room.hpp"
#include "client.hpp"
#include "error.hpp"
#include <fmt/format.h>

using namespace matrix;

room::room(client &client_, const std::string &id) : id(id) {
  http_client = client_.create_http_client();
  on_sync_connection = client_.on_sync.connect(
      [this](const auto &content) { on_sync(content); });
}

const std::string &room::get_id() const { return id; }

boost::future<void> room::invite_by_user_id(const std::string &user_id) {
  BOOST_ASSERT(!user_id.empty());
  auto target = fmt::format("rooms/{}/invite", id);
  auto request = nlohmann::json::object();
  request["user_id"] = user_id;
  return http_client->post(target, request).then(executor, [](auto result) {
    auto got_result = result.get();
    error::check_matrix_response(got_result.first, got_result.second);
  });
}

const room::members_type &room::get_members() const { return members; }

void room::on_sync(const nlohmann::json &content) {
  const auto joined_rooms = content["rooms"]["join"];
  if (!joined_rooms.contains(id))
    return;
  auto our_room = joined_rooms[id];
  auto events = our_room["timeline"]["events"];
  for (const auto &event : events) {
    const std::string type = event["type"];
    if (type == "m.room.member") {
      on_event_join(event);
      continue;
    }
  }
}

void room::on_event_join(const nlohmann::json &parse) {
  // https://matrix.org/docs/spec/client_server/latest#m-room-member
  // Adjusts the membership state for a user in a room.
  const std::string user_id = parse["state_key"];
  const auto content = parse["content"];
  const std::string membership = content["membership"];
  if (membership == "leave")
    return remove_member(user_id);
  if (membership == "join") {
    member signal_argument;
    signal_argument.display_name = content["displayname"];
    signal_argument.id = user_id;
    return add_member(signal_argument);
  }
  // lots more - not implemented
}

void room::add_member(const member &member_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("add_member, id:'{}', display_name:'{}'", member_.id,
                     member_.display_name);
  BOOST_ASSERT(members.cend() ==
               std::find_if(members.cbegin(), members.cend(), [&](auto &check) {
                 return check.id == member_.id;
               }));
  members.push_back(member_);
  on_join(member_);
}

void room::remove_member(const std::string &user_id) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("remove_member, id:'{}'", user_id);
  const auto found =
      std::find_if(members.cbegin(), members.cend(),
                   [&](auto &check) { return check.id == user_id; });
  if (found == members.cend()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "cant remove user because not found, user_id:" << user_id;
    BOOST_ASSERT(false);
    return;
  }
  members.erase(found);
  on_leave(user_id);
}
