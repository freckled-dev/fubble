#ifndef UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B
#define UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B

#include "session/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace session {
class client;
class client_connector {
public:
  client_connector(client &client_);
  boost::future<void> connect();

protected:
  session::logger logger{"client_connector"};
  client &client_;
  boost::promise<void> promise;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace session

#endif
