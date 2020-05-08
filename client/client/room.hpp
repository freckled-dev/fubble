#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include "client/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class client;
class room;
class user;
} // namespace matrix

namespace client {
class participant;
class participant_creator;
class room {
public:
  // TODO refactor. client shall not be owned by room
  room(std::unique_ptr<participant_creator> participant_creator_,
       std::unique_ptr<matrix::client> client_, matrix::room &room_);
  ~room();

  // these signals could be in a class calles participants. Do so if room grows
  boost::signals2::signal<void(const std::vector<participant *> &added)>
      on_participants_join;
  boost::signals2::signal<void(const std::vector<std::string> &removed)>
      on_participants_left;
  std::vector<participant *> get_participants() const;
  boost::signals2::signal<void(const std::string &)> on_name_changed;

  std::string get_name() const;
  std::string get_own_id() const;
  boost::future<void> leave();

protected:
  void on_session_participant_joins(const std::deque<matrix::user *> &joins);
  void on_session_participant_leaves(const std::vector<std::string> &leaves);
  using participants = std::vector<std::unique_ptr<participant>>;
  participants::iterator find(const std::string &id);

  client::logger logger;
  std::unique_ptr<participant_creator> participant_creator_;
  boost::inline_executor executor;
  std::unique_ptr<matrix::client> client_;
  matrix::room &room_;
  participants participants_;
};
} // namespace client

#endif
