#include "room.hpp"
#include "chat.hpp"
#include "client.hpp"
#include "error.hpp"
#include "events.hpp"
#include "room_participant.hpp"
#include "room_states.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using namespace matrix;

room::room(client &client_, const std::string &id)
    : client_(client_), id(id), chat_{std::make_unique<chat>(client_, id)},
      states_{room_states::create(client_, id)},
      event_parser_{event_parser::create(client_.get_users())} {
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

room_states &room::get_states() const { return *states_; }

void room::sync(const nlohmann::json &content) {
  const auto joined_rooms = content["rooms"]["join"];
  BOOST_ASSERT(joined_rooms.contains(id));
  if (!joined_rooms.contains(id))
    return;
  auto our_room = joined_rooms[id];
  // check note `The /sync API returns a state list which is separate from the
  // timeline.` for the difference between state and timeline
  // https://matrix.org/docs/spec/client_server/latest#syncing
  auto timeline_events = our_room["timeline"]["events"];
  on_events(timeline_events);
  auto state_events = our_room["state"]["events"];
  on_events(state_events);
}

void room::on_events(const nlohmann::json &events) {
  for (const auto &event : events) {
    auto parsed = event_parser_->parse(event);
    if (parsed) {
      auto state_event = dynamic_cast<event::room_state_event *>(parsed.get());
      if (state_event)
        states_->sync_event(*state_event);
    }
    const std::string type = event["type"];
    if (chat_->sync_event(type, event))
      continue;
    if (type == "m.room.member") {
      on_event_m_room_member(event);
      continue;
    }
    if (type == "m.room.name") {
      BOOST_LOG_SEV(logger, logging::severity::debug) << "m.room.name";
      on_event_m_room_name(event);
      continue;
    }
  }
}

void room::on_event_m_room_member(const nlohmann::json &parse) {
  // https://matrix.org/docs/spec/client_server/latest#m-room-member
  // Adjusts the membership state for a user in a room.
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_event_m_room_member";
  const std::string user_id = parse["state_key"];
  const std::int64_t origin_server_ts = parse["origin_server_ts"];
  std::chrono::milliseconds origin_server_ts_casted{origin_server_ts};
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
  std::chrono::system_clock::time_point timestamp_casted{
      origin_server_ts_casted};
  user &add_or_update = client_.get_users().get_or_add_user(user_id);
  if (display_name)
    add_or_update.set_display_name(display_name.value());

  auto got = get_member_by_id(user_id);
  if (got)
    return got.value()->set_join_state(join_state_, timestamp_casted);

  auto add = std::make_unique<room_participant>(add_or_update, join_state_,
                                                timestamp_casted);
  members.push_back(std::move(add));
  on_join(*members.back());
}

void room::on_event_m_room_name(const nlohmann::json &parse) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_event_m_room_name";
  name = parse["content"]["name"];
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "room_name changed to:" << name;
  on_name_changed(name);
}
