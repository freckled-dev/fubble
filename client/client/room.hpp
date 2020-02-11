#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include "logging/logger.hpp"
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
  room(std::unique_ptr<participant_creator> participant_creator_,
       std::unique_ptr<session::client> client_,
       std::unique_ptr<session::room> room_);
  ~room();

  // these signals could be in a class calles participants. Do so if room grows
  boost::signals2::signal<void(const std::vector<participant *> &added)>
      on_participants_join;
  boost::signals2::signal<void(const std::vector<participant *> &updated)>
      on_participants_updated;
  boost::signals2::signal<void(const std::vector<std::string> &removed)>
      on_participants_left;
  std::vector<participant *> get_participants() const;

  const std::string &get_name() const;
  const std::string &get_own_id() const;

protected:
  void
  on_session_participant_joins(const std::vector<session::participant> &joins);
  void on_session_participant_updates(
      const std::vector<session::participant> &updates);
  void on_session_participant_leaves(const std::vector<std::string> &leaves);
  using participants = std::vector<std::unique_ptr<participant>>;
  participants::iterator find(const std::string &id);

  logging::logger logger;
  std::unique_ptr<participant_creator> participant_creator_;
  std::unique_ptr<session::client> client_;
  std::unique_ptr<session::room> room_;
  participants participants_;
};
} // namespace client

#endif
