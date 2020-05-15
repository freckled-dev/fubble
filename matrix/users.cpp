#include "users.hpp"
#include "client.hpp"
#include <nlohmann/json.hpp>

using namespace matrix;

users::users(client &client_) : client_(client_) {
  on_sync_connection = client_.on_sync.connect(
      [this](const auto &content) { on_sync(content); });
}

user &users::get_or_add_user(const std::string &id) {
  auto found = find(id);
  if (found != users_.end())
    return *found->get();
  auto user_ = std::make_unique<user>(id);
  users_.emplace_back(std::move(user_));
  return *users_.back();
}

user &users::get_by_id(const std::string &id) {
  auto found = find(id);
  if (found == users_.end()) {
    BOOST_ASSERT(false);
  }
  return *found->get();
}

void users::on_sync(const nlohmann::json &content) {
  const auto presence = content["presence"];
  const auto events = presence["events"];
  for (const auto &event : events) {
    const std::string type = event["type"];
    if (type != "m.presence")
      continue;
    const std::string user_id = event["sender"];
    user &user_ = get_or_add_user(user_id);
    user_.on_m_presence(event);
  }
}

std::deque<std::unique_ptr<user>>::iterator users::find(const std::string &id) {
  return std::find_if(users_.begin(), users_.end(),
                      [&](auto &check) { return check->get_id() == id; });
}
