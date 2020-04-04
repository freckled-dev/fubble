#ifndef UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B
#define UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B

#include "client_factory.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "session/logger.hpp"

namespace session {
class client;
class client_connector {
public:
  client_connector(client_factory &client_factory,
                   matrix::authentification &matrix_authentification);
  using client_ptr = std::unique_ptr<client>;
  boost::future<client_ptr> connect();

protected:
  session::logger logger{"client_connector"};
  client_factory &client_factory_;
  matrix::authentification &matrix_authentification;
  std::shared_ptr<boost::promise<client_ptr>> promise;
};
} // namespace session

#endif
