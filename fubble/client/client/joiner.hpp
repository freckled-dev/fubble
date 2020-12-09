#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "client/logger.hpp"
#include "utils/exception.hpp"
#include "version/getter.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class authentification;
class client;
class client_synchronizer;
} // namespace matrix
namespace temporary_room::net {
class client;
} // namespace temporary_room::net

namespace client {
class room;
class rooms;
class room_creator;
class joiner {
public:
  struct update_required : utils::exception {};
  using minimum_version_info =
      boost::error_info<struct minimum_version_tag, std::string>;
  using current_version_info =
      boost::error_info<struct current_version_tag, std::string>;
  using installed_version_info =
      boost::error_info<struct installed_version_tag, std::string>;

  joiner(room_creator &room_creator_, rooms &rooms_,
         matrix::authentification &matrix_authentification,
         temporary_room::net::client &temporary_room_client,
         std::shared_ptr<version::getter> version_getter,
         std::shared_ptr<matrix::client_synchronizer> syncher);
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
  std::shared_ptr<matrix::client_synchronizer> client_synchronizer;
};
} // namespace client

#endif
