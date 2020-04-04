#ifndef UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E
#define UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E

#include "matrix/client.hpp"
#include "session/logger.hpp"

namespace session {
class room;
class client {
public:
  client(std::unique_ptr<matrix::client> matrix_client);
  ~client();

  boost::future<void> run();
  void close();
  boost::future<void> set_name(const std::string &name);
  // TODO do a `rooms` class
  boost::future<void> leave_room(room &room_);
  std::string get_id() const;

  const matrix::client &get_natives() const;
  matrix::client &get_natives();

protected:
  session::logger logger{"client"};
  std::unique_ptr<matrix::client> matrix_client;
};
} // namespace session

#endif
