#ifndef UUID_0BAA8A31_4C07_49DD_873A_C04A04E2D4FC
#define UUID_0BAA8A31_4C07_49DD_873A_C04A04E2D4FC

#include <boost/signals2/signal.hpp>
#include <chrono>
#include <nlohmann/json.hpp>
#include <boost/optional.hpp>
#include <string>

// TODO think. Is this the wrong way?
// TODO this class/module is not all implementated yet
// currently room, user and so parse the events it themself and set an intern
// state. event_parser and event namespace would be a dedicated central
// implementation point.
namespace matrix {
class client;
class user;
class users;
namespace event {
struct event {
public:
  virtual ~event() = default;
  std::string type;
};
using event_ptr = std::shared_ptr<event>;
namespace room {
struct content {
  virtual ~content() = default;
};
struct custom_state : content {
  nlohmann::json data;
};
struct message : content {
  std::string type;
  std::string body;
};
} // namespace room
struct room_event : event {
  std::string event_id;
  boost::optional<std::string> room_id;
  std::chrono::system_clock::time_point origin_server_ts;
  user *sender{};
  std::unique_ptr<room::content> content_;
};
// https://matrix.org/docs/spec/client_server/r0.6.1#state-event-fields
struct room_state_event : room_event {
  std::string state_key;
};
} // namespace event

class event_parser {
public:
  virtual ~event_parser() = default;

  virtual std::unique_ptr<event::event>
  parse(const nlohmann::json &event) const = 0;

  static std::unique_ptr<event_parser> create(matrix::users &users);
};

class events {
public:
  virtual ~events() = default;

  virtual bool handle_event(const event::event_ptr &handle) = 0;

  boost::signals2::signal<void(event::event_ptr)> on_event;

  using events_container = std::vector<event::event_ptr>;
  virtual events_container get_events() const = 0;

  static std::unique_ptr<events> create_all_events();
  static std::unique_ptr<events> create_room_filter(events &events_,
                                                    const std::string &room_id);
};
// TODO class state_events
// https://matrix.org/docs/spec/client_server/latest#state-event-fields
} // namespace matrix

#endif
