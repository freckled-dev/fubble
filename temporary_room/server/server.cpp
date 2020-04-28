#include "server.hpp"

using namespace temporary_room::server;

server::server(temporary_room::net::server::server &net_server,
              temporary_room::rooms::rooms &rooms)
    : net_server(net_server), rooms(rooms) {
  net_server.on_join = [this](const auto &name, const auto &user_id) {
    return on_join(name, user_id);
  };
}

boost::future<std::string> server::on_join(const std::string &name,
                                           const std::string &user_id) {
  return rooms.get_or_create_room_id(name, user_id);
}
