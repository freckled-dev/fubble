#include "server.hpp"

using namespace temporary_room::server;

server::server(net::server::server &net_server, rooms::rooms &rooms)
    : net_server(net_server), rooms(rooms) {
  net_server.on_join = [this](const auto &name) { return on_join(name); };
}

boost::future<std::string> server::on_join(const std::string &name) {
  // rooms.add_or_update(name);
  (void)name;
  return boost::make_exceptional_future<std::string>(
      std::runtime_error("not implemented"));
}
