#ifndef UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE
#define UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE

#include "logger.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace temporary_room::rooms {

using room_id = std::string;
using room_name = std::string;
using user_id = std::string;

class room {
public:
  virtual ~room() = default;
  virtual bool is_empty() const = 0;
  std::function<void()> on_empty;
  virtual room_id get_room_id() const = 0;
  virtual room_name get_room_name() const = 0;
  virtual boost::future<void> invite(const user_id &) = 0;
};
using room_ptr = std::shared_ptr<room>;

// TODO find a better name. `room_source`?
class room_factory {
public:
  virtual ~room_factory() = default;
  virtual boost::future<room_ptr> create(const std::string &name) = 0;
  std::function<void(room_ptr)> on_room;
};

class rooms {
public:
  rooms(room_factory &factory);
  ~rooms();

  boost::future<room_id> get_or_create_room_id(const room_name &name,
                                               const user_id &user_id_);
  std::size_t get_room_count();

protected:
  struct participant {
    std::shared_ptr<boost::promise<room_id>> promise =
        std::make_shared<boost::promise<room_id>>();
    user_id user_id_;
    room_id room_id_;

    participant(const user_id &user_id_) : user_id_(user_id_) {}
  };

  void on_room(room_ptr room_);
  void create(const room_name &name);
  void on_created(const room_name &name, boost::future<room_ptr> &result);
  void on_new_room(room_ptr room_);
  void on_empty(const room_name &name);
  void invite(const std::shared_ptr<participant> participant_,
              const room_ptr &room_);
  void on_invited(const std::shared_ptr<participant> &participant_,
                  boost::future<void> result);

  room_factory &factory;
  boost::inline_executor executor;
  temporary_room::logger logger{"rooms"};
  struct room_adapter {
    room_ptr room_;
    std::vector<std::shared_ptr<participant>> waiting_for_room;
  };
  std::unordered_map<room_name, room_adapter> rooms_;
};
} // namespace temporary_room::rooms

#endif
