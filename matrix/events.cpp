#include "events.hpp"
#include "matrix/logger.hpp"
#include "matrix/users.hpp"
#include <nlohmann/json.hpp>

using namespace matrix;

namespace {
class event_parser_impl : public event_parser {
public:
  event_parser_impl(users &users_) : users_(users_) {}

  std::unique_ptr<event::event>
  parse(const nlohmann::json &event_) const override {
    std::string type = event_["type"];
    if (type == "m.room.message")
      return on_event_m_room_message(event_);
    return nullptr;
  }

  void parse_room_event(event::room_event &set,
                        const nlohmann::json &event) const {
    std::int64_t timestamp_milliseconds = event["origin_server_ts"];
    std::chrono::milliseconds timestamp_milliseconds_casted{
        timestamp_milliseconds};
    set.origin_server_ts =
        std::chrono::system_clock::time_point{timestamp_milliseconds_casted};
    std::string user_id = event["sender"];
    auto &user_ = users_.get_or_add_user(user_id);
    set.sender = &user_;
    set.room_id = event["room_id"];
    set.event_id = event["event_id"];
  }

  std::unique_ptr<event::room_event>
  on_event_m_room_message(const nlohmann::json &event) const {
    // https://matrix.org/docs/spec/client_server/latest#m-text
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    auto result = std::make_unique<event::room_event>();
    parse_room_event(*result, event);
    auto content = event["content"];
    auto result_content = std::make_unique<event::room::message>();
    result_content->type = content["msgtype"];
    if (result_content->type != "m.text") {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << "a message with the type:" << result_content->type
          << ", could not get parsed";
      return {};
    }
    result_content->body = content["body"];
    result->content_ = std::move(result_content);
    return result;
  }

protected:
  mutable matrix::logger logger{"event_parser_impl"};
  users &users_;
};

class all_events : public events {
public:
  all_events() {}

  bool handle_event(const std::shared_ptr<event::event> &handle) override {
    events_.push_back(handle);
    on_event(handle);
    return true;
  }

  events_container get_events() const override { return events_; }

protected:
  matrix::logger logger{"events_impl"};
  events_container events_;
};
class room_filter : public events {
public:
  room_filter(events &events_, const std::string &room_id)
      : source(events_), room_id{room_id} {}

  bool handle_event(const std::shared_ptr<event::event> &handle) override {
    (void)handle;
    return false;
  }

  events_container get_events() const override { return events_; }

protected:
  matrix::logger logger{"room_filter"};
  events &source;
  const std::string room_id;
  events_container events_;
};
} // namespace

std::unique_ptr<event_parser> event_parser::create(matrix::users &users_) {
  return std::make_unique<event_parser_impl>(users_);
}

std::unique_ptr<events> events::create_all_events() {
  return std::make_unique<all_events>();
}

std::unique_ptr<events> events::create_room_filter(events &events_,
                                                   const std::string &room_id) {
  return std::make_unique<room_filter>(events_, room_id);
}
