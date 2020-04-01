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
class room_joiner {
public:
  room_joiner(temporary_room::net::client &temporary_room_client,
              client &client_);
  ~room_joiner();
  using room_ptr = std::unique_ptr<room>;
  boost::future<room_ptr> join(const std::string &room);

protected:
  boost::future<matrix::room *>
  on_got_invited(boost::future<std::string> &result);
  room_ptr on_joined(boost::future<matrix::room *> &result);

  session::logger logger{"room_joiner"};
  boost::inline_executor executor;
  client &client_;
  temporary_room::net::client &temporary_room_client;
};
} // namespace session

#endif
