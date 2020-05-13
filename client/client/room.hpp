#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include "client/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <deque>

namespace matrix {
class client;
class room;
class room_participant;
class user;
} // namespace matrix

namespace client {
class chat;
class participant;
class participant_creator;
class participants;
class users;
class room {
public:
  // TODO refactor. client shall not be owned by room
  room(std::unique_ptr<participant_creator> participant_creator_,
       std::unique_ptr<matrix::client> client_, matrix::room &room_);
  ~room();

  chat &get_chat() const;
  participants &get_participants() const;
  users &get_users() const;
  boost::signals2::signal<void(const std::string &)> on_name_changed;

  std::string get_name() const;
  std::string get_own_id() const;
  boost::future<void> leave();

protected:
  void on_session_participant_joins(
      const std::vector<matrix::room_participant *> &joins);
  void on_session_participant_leaves(const std::vector<std::string> &leaves);

  client::logger logger;
  std::unique_ptr<participant_creator> participant_creator_;
  boost::inline_executor executor;
  std::unique_ptr<matrix::client> client_;
  matrix::room &room_;
  std::unique_ptr<chat> chat_;
  std::unique_ptr<users> users_;
  std::unique_ptr<participants> participants_;
};
} // namespace client

#endif
