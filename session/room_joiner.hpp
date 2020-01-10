#ifndef UUID_8B24544C_F862_445E_8A04_513A53E210B0
#define UUID_8B24544C_F862_445E_8A04_513A53E210B0

#include <boost/thread/future.hpp>
#include <memory>
#include <nakama-cpp/Nakama.h>

namespace session {
class room;
class client;
class room_joiner : public std::enable_shared_from_this<room_joiner> {
public:
  room_joiner(client &client_);
  using room_ptr = std::unique_ptr<room>;
  boost::future<room_ptr> join(const std::string &room);

protected:
  void on_success(Nakama::NChannelPtr channel);
  void on_error(Nakama::NRtError error);

  client &client_;
  boost::promise<room_ptr> promise;
};
} // namespace session

#endif
