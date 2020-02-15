#ifndef UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B
#define UUID_94FB80F2_F8BE_4C79_8304_921A81B5899B

#include "session/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
#include <nakama-cpp/Nakama.h>

namespace session {
class client;
class client_connector {
public:
  client_connector(client &client_);
  boost::future<void> connect();

protected:
  void on_logged_in(const Nakama::NSessionPtr &session_);
  void on_login_failed(const Nakama::NError &error);
  void on_realtime_connected();
  void on_realtime_error(const Nakama::NRtError &error);

  session::logger logger{"client_connector"};
  client &client_;
  boost::promise<void> promise;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace session

#endif
