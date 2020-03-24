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
class room {
public:
  room(client &client_, const std::string &id);
  const std::string &get_id() const;
  boost::future<void> invite_by_user_id(const std::string &user_id);
  void kick();
  struct member {
    std::string id;
    std::string display_name;
  };
  using members_type = std::deque<member>;
  const members_type &get_members() const;

  boost::signals2::signal<void(const member &)> on_join;
  boost::signals2::signal<void(const std::string &)> on_leave;

protected:
  void on_sync(const nlohmann::json &content);
  void on_event_join(const nlohmann::json &parse);
  void add_member(const member &member_);
  void remove_member(const std::string &member_);

  matrix::logger logger{"room"};
  boost::inline_executor executor;
  const std::string id;
  std::unique_ptr<http::client> http_client;
  boost::signals2::scoped_connection on_sync_connection;
  members_type members;
};
} // namespace matrix

#endif
