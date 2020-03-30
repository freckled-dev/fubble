#ifndef UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E
#define UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E

#include "matrix/client.hpp"
#include "session/logger.hpp"

namespace session {
class client {
public:
  client(matrix::client &matrix_client);
  ~client();

  void close();
  boost::future<void> set_name(const std::string &name);

  const matrix::client &get_natives() const;
  matrix::client &get_natives();

protected:
  session::logger logger{"client"};
  matrix::client &matrix_client;
};
} // namespace session

#endif
