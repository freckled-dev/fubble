#include "user.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using namespace matrix;

user::user(const std::string &id)
    : logger{fmt::format("user {}", id)}, id(id) {}

void user::on_m_presence(const nlohmann::json &event) {
  const auto event_content = event["content"];
  std::string presence = event_content["presence"];
  set_presence_from_string(presence);
  if (event_content.contains("displayname"))
    set_display_name(event_content["displayname"]);
}

void user::set_presence(const presence &presence_parameter) {
  if (presence_ == presence_parameter)
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "set_presence, presence:" << presence_parameter;
  presence_ = presence_parameter;
  on_update();
}

void user::set_presence_from_string(const std::string &presence_parameter) {
  if (presence_parameter == "online")
    return set_presence(presence::online);
  if (presence_parameter == "offline")
    return set_presence(presence::offline);
  if (presence_parameter == "unavailable")
    return set_presence(presence::unavailable);
  BOOST_ASSERT(false);
}

void user::set_display_name(const std::string &display_name_) {
  if (display_name == display_name_)
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", current:" << display_name
      << ", new:" << display_name_;
  display_name = display_name_;
  on_update();
}
