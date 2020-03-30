#ifndef UUID_F3BD90D4_472D_4BB3_9675_43C5F69083FA
#define UUID_F3BD90D4_472D_4BB3_9675_43C5F69083FA

#include "logger.hpp"
#include "room.hpp"
#include <boost/signals2/signal.hpp>
#include <deque>
#include <nlohmann/json.hpp>

namespace matrix {
class client;
class factory;
class rooms {
public:
  rooms(factory &factory_, client &client_);

  boost::future<room *> create_room();
  boost::future<void> leave_room(const room &room_);
  boost::future<void> leave_room_by_id(const std::string &id);
  boost::future<room *> join_room_by_id(const std::string &id);
  using room_list = std::deque<std::unique_ptr<room>>;
  inline const room_list &get_rooms() { return rooms_; }
  std::optional<room *> get_room_by_id(const std::string &id);

  boost::signals2::signal<void(const std::string &)> on_leave;

protected:
  void on_sync(const nlohmann::json &content);
  void on_room_created(std::unique_ptr<room> room_);

  matrix::logger logger{"rooms"};
  factory &factory_;
  client &client_;
  boost::inline_executor executor;
  boost::signals2::scoped_connection on_sync_connection;
  std::unique_ptr<http::client> http_client;
  room_list rooms_;
};
} // namespace matrix

#endif
