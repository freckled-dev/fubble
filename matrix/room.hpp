#ifndef UUID_89583835_54BA_4B8D_AD82_A68806B04398
#define UUID_89583835_54BA_4B8D_AD82_A68806B04398

#include "http/client.hpp"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <deque>
#include <string>

namespace matrix {
class client;
class user;
class room {
public:
  room(client &client_, const std::string &id);
  const std::string &get_id() const;
  boost::future<void> invite_by_user_id(const std::string &user_id);
  boost::future<void> kick(const std::string &user_id);
  using members_type = std::deque<user *>;
  const members_type &get_members() const;
  std::optional<user *> get_member_by_id(const std::string &id);

  boost::signals2::signal<void(const user &)> on_join;
  boost::signals2::signal<void(const std::string &)> on_leave;

protected:
  struct member {
    std::string id;
    std::string display_name;
  };
  void on_sync(const nlohmann::json &content);
  void on_event_m_room_member(const nlohmann::json &parse);
  void add_or_update_member(const member &member_);
  void remove_member(const std::string &member_);

  matrix::logger logger{"room"};
  boost::inline_executor executor;
  client &client_;
  const std::string id;
  std::unique_ptr<http::client> http_client;
  boost::signals2::scoped_connection on_sync_connection;
  members_type members;
};
} // namespace matrix

#endif
