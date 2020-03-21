#ifndef UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE
#define UUID_10C55C77_A55C_4A79_B99E_96FD62EF44DE

#include "logger.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace temporary_room::rooms {

using room_id = std::string;
using room_name = std::string;

class room_factory {
public:
  virtual ~room_factory() = default;
  virtual boost::future<room_id> create() = 0;
  virtual void destroy(const room_id &id) = 0;
};

class participant {
public:
  virtual ~participant() = default;
  virtual void set_room(const room_id &id) = 0;
  virtual void set_error(const std::runtime_error &error) = 0;
};

class rooms {
public:
  rooms(room_factory &factory);
  ~rooms();

  void add_participant(participant &add, const room_name &name);
  void remove_participant(participant &remove, const room_name &name);
  std::size_t get_room_count();

protected:
  void create(const room_name &name);
  void on_created(const room_name &name, boost::future<room_id> &result);
  void destroy(const room_id &id);

  room_factory &factory;
  boost::inline_executor executor;
  temporary_room::logger logger{"rooms"};

  struct room {
    std::optional<room_id> id;
    std::vector<participant *> participants;
  };
  std::unordered_map<room_name, room> rooms_;
};
} // namespace temporary_room::rooms

#endif
