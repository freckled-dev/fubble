#ifndef UUID_849EB9FB_68E0_4428_9C7C_A81564E9E24D
#define UUID_849EB9FB_68E0_4428_9C7C_A81564E9E24D

#include "acceptor.hpp"

namespace temporary_room::net::server {
class server {
public:
  server(net::server::acceptor &net_server);

  std::function<boost::future<std::string>(std::string name)> on_join;

protected:
  response_future on_request(const std::string &target);

  net::server::acceptor &acceptor_;
};
} // namespace temporary_room::net::server

#endif
