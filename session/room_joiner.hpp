#ifndef UUID_8B24544C_F862_445E_8A04_513A53E210B0
#define UUID_8B24544C_F862_445E_8A04_513A53E210B0

#include "session/logger.hpp"
#include <boost/thread/future.hpp>

namespace session {
class room;
class client;
class room_joiner {
public:
  room_joiner(client &client_);
  using room_ptr = std::unique_ptr<room>;
  boost::future<room_ptr> join(const std::string &room);

protected:
  session::logger logger{"room_joiner"};
  client &client_;
};
} // namespace session

#endif
