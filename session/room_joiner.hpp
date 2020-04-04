#ifndef UUID_8B24544C_F862_445E_8A04_513A53E210B0
#define UUID_8B24544C_F862_445E_8A04_513A53E210B0

#include "matrix/room.hpp"
#include "session/logger.hpp"
#include "temporary_room/net/client.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace session {
class room;
class client;
// TODO refactor to temporary_room_joiner
class room_joiner {
public:
  room_joiner(temporary_room::net::client &temporary_room_client);
  ~room_joiner();
  using room_ptr = std::unique_ptr<room>;
  // TODO take shared_ptr of client!
  boost::future<room_ptr> join(client &client_, const std::string &room);

protected:
  boost::future<matrix::room *>
  on_got_invited(client &client_, boost::future<std::string> &result);
  room_ptr on_joined(boost::future<matrix::room *> &result);

  session::logger logger{"room_joiner"};
  boost::inline_executor executor;
  temporary_room::net::client &temporary_room_client;
};
} // namespace session

#endif
