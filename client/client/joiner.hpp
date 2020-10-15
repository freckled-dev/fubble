#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "client/logger.hpp"
#include "version/getter.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class authentification;
class client;
} // namespace matrix
namespace temporary_room::net {
class client;
}

namespace client {
class room;
class rooms;
class room_creator;
class joiner {
public:
  joiner(room_creator &room_creator_, rooms &rooms_,
         matrix::authentification &matrix_authentification,
         temporary_room::net::client &temporary_room_client,
         std::shared_ptr<version::getter> version_getter);
  ~joiner();

  struct parameters {
    std::string name;
    std::string room;
  };
  // thread-safe
  using room_ptr = std::shared_ptr<room>;
  boost::future<room_ptr> join(const parameters &parameters_);

protected:
  room_ptr on_joined(boost::future<room_ptr> &from_joiner);

  client::logger logger{"joiner"};
  boost::inline_executor executor;
  room_creator &room_creator_;
  rooms &rooms_;
  matrix::authentification &matrix_authentification;
  temporary_room::net::client &temporary_room_client;
  std::shared_ptr<version::getter> version_getter;
};
} // namespace client

#endif
