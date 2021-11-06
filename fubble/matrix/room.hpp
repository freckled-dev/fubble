#ifndef UUID_89583835_54BA_4B8D_AD82_A68806B04398
#define UUID_89583835_54BA_4B8D_AD82_A68806B04398

#include "fubble/http/client.hpp"
#include "logger.hpp"
#include <sigslot/signal.hpp>
#include <sigslot/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <string>

namespace matrix {
class chat;
class client;
class event_parser;
class room_participant;
class room_states;
class user;
class room {
public:
  room(client &client_, const std::string &id);
  ~room();
  const std::string &get_id() const;
  boost::future<void> invite_by_user_id(const std::string &user_id);
  boost::future<void> kick(const std::string &user_id);
  std::vector<room_participant *> get_members() const;
  boost::optional<room_participant *> get_member_by_id(const std::string &id);
  boost::optional<std::string> get_name() const;
  chat &get_chat() const;
  room_states &get_states() const;

  // TODO refactor to `on_added`
  sigslot::signal<room_participant &> on_join;
  sigslot::signal<const std::string &> on_name_changed;

  void sync(const nlohmann::json &content);

protected:
  void on_events(const nlohmann::json &events);
  void on_event_m_room_member(const nlohmann::json &parse);
  void on_event_m_room_name(const nlohmann::json &parse);

  matrix::logger logger{"room"};
  boost::inline_executor executor;
  client &client_;
  const std::string id;
  std::unique_ptr<http::client> http_client;
  sigslot::scoped_connection on_sync_connection;
  using members_type = std::vector<std::unique_ptr<room_participant>>;
  members_type members;
  boost::optional<std::string> name;
  const std::unique_ptr<chat> chat_;
  const std::unique_ptr<room_states> states_;
  const std::unique_ptr<event_parser> event_parser_;
};
} // namespace matrix

#endif
