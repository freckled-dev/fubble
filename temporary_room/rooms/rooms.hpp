#ifndef UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE
#define UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE

#include "logger.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace temporary_room::rooms {

using room_id = std::string;
using room_name = std::string;

class room {
public:
  virtual ~room() = default;
  std::function<void(int)> on_participant_count_changed;
  virtual room_id get_room_id() const = 0;
};
using room_ptr = std::unique_ptr<room>;

class room_factory {
public:
  virtual ~room_factory() = default;
  virtual boost::future<room_ptr> create() = 0;
};

class participant {
public:
  std::shared_ptr<boost::promise<room_id>> promise =
      std::make_shared<boost::promise<room_id>>();

  virtual ~participant() = default;
  inline void set_room(const room_id &id) {
    auto keep_alive = promise;
    promise->set_value(id);
  }
  inline void set_error(const std::runtime_error &error) {
    auto keep_alive = promise;
    promise->set_exception(error);
  }
};

class rooms {
public:
  rooms(room_factory &factory);
  ~rooms();

  boost::future<room_id> get_or_create_room_id(const room_name &name);
  std::size_t get_room_count();

protected:
  void create(const room_name &name);
  void on_created(const room_name &name, boost::future<room_ptr> &result);
  void on_participant_count_changed(const room_name &name, const int count);

  room_factory &factory;
  boost::inline_executor executor;
  temporary_room::logger logger{"rooms"};

  struct room_adapter {
    room_ptr room_;
    std::vector<std::unique_ptr<participant>> participants;
  };
  std::unordered_map<room_name, room_adapter> rooms_;
};
} // namespace temporary_room::rooms

#endif
