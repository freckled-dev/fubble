#ifndef UUID_F3BD90D4_472D_4BB3_9675_43C5F69083FA
#define UUID_F3BD90D4_472D_4BB3_9675_43C5F69083FA

#include "logger.hpp"
#include "room.hpp"
#include <fubble/utils/signal.hpp>
#include <deque>
#include <nlohmann/json_fwd.hpp>

namespace matrix {
class client;
class factory;
class rooms {
public:
  rooms(factory &factory_, client &client_);

  struct create_room_fields {
    boost::optional<std::string> name;
  };
  boost::future<room *> create_room(const create_room_fields &fields);
  // TODO refactor to `leave`
  boost::future<void> leave_room(const room &room_);
  // TODO refactor to `leave_by_id`
  boost::future<void> leave_room_by_id(const std::string &id);
  // TODO refactor to `join_by_id`
  boost::future<room *> join_room_by_id(const std::string &id);
  using room_list = std::deque<std::unique_ptr<room>>;
  // TODO refactor to `get_all`
  inline const room_list &get_rooms() const { return rooms_; }
  boost::optional<room *> get_room_by_id(const std::string &id) const;

  utils::signal::signal<room &> on_joined;
  utils::signal::signal<const std::string &> on_leave;

protected:
  void on_sync(const nlohmann::json &content);
  void on_room_created(std::unique_ptr<room> room_);
  room_list::iterator find_room_by_id(const std::string &id);
  room &do_create_room(const std::string &id);

  matrix::logger logger{"rooms"};
  factory &factory_;
  client &client_;
  boost::inline_executor executor;
  utils::signal::scoped_connection on_sync_connection;
  std::unique_ptr<http::client> http_client;
  room_list rooms_;
};
} // namespace matrix

#endif
