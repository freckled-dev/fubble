#ifndef UUID_1FEA638C_1FFA_48B8_AB98_BF6A851F3838
#define UUID_1FEA638C_1FFA_48B8_AB98_BF6A851F3838

#include "participant_creator_creator.hpp"
#include <boost/thread/executor.hpp>

namespace session {
class client;
class room;
} // namespace session

namespace client {
class room;
class room_creator {
public:
  room_creator(participant_creator_creator &participant_creator_creator_);

  std::unique_ptr<room> create(std::unique_ptr<session::client> client_,
                               std::unique_ptr<session::room> room_);

protected:
  participant_creator_creator &participant_creator_creator_;
};
} // namespace client

#endif