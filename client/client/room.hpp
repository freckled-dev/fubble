#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include "session/participant.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executor.hpp>

namespace session {
class client;
class room;
} // namespace session

namespace client {
class participant;
class participant_creator;
class room {
public:
  room(boost::executor &session_thread,
       participant_creator &participant_creator_,
       std::unique_ptr<session::client> client_,
       std::unique_ptr<session::room> room_);
  ~room();

  // these signals could be in a class calles participants. Do so if room grows
  boost::signals2::signal<void(const std::vector<participant *> &added)>
      on_participants_join;
  boost::signals2::signal<void(const std::vector<std::string> &removed)>
      on_participants_left;

  const std::string &get_name() const;
  // TODO this seems stupid!
  boost::executor &get_session_thread();

protected:
  void
  on_session_participant_joins(const std::vector<session::participant> &joins);
  void on_session_participant_leaves(const std::vector<std::string> &leaves);

  boost::executor &session_thread;
  participant_creator &participant_creator_;
  std::unique_ptr<session::client> client_;
  std::unique_ptr<session::room> room_;
  std::vector<std::unique_ptr<participant>> participants;
};
} // namespace client

#endif
