#ifndef UUID_73DE619B_B268_48C3_BDB5_5A6C948A49E6
#define UUID_73DE619B_B268_48C3_BDB5_5A6C948A49E6

#include "fubble/temporary_room/net/server.hpp"
#include "fubble/temporary_room/rooms/rooms.hpp"

namespace temporary_room::server {
class server {
public:
  server(net::server::server &net_server, rooms::rooms &rooms);

protected:
  boost::future<std::string> on_join(const std::string &name,
                                     const std::string &user_id);

  net::server::server &net_server;
  rooms::rooms &rooms;
};
} // namespace temporary_room::server

#endif
