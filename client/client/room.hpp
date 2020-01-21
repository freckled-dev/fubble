#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include <boost/signals2/signal.hpp>
#include <boost/thread/executor.hpp>

namespace session {
class client;
class room;
} // namespace session

namespace client {
class room {
public:
  room(boost::executor &session_thread,
       std::unique_ptr<session::client> client_,
       std::unique_ptr<session::room> room_);
  ~room();

  session::room &get_session_room();
  boost::executor &get_session_thread();

protected:
  boost::executor &session_thread;
  std::unique_ptr<session::client> client_;
  std::unique_ptr<session::room> room_;
};
} // namespace client

#endif
