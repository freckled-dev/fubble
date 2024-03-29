#ifndef UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A
#define UUID_D136BB60_81E5_4336_A54C_72FACE9EE23A

#include "fubble/client/logger.hpp"
#include <fubble/utils/signal.hpp>
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
       std::shared_ptr<matrix::client> client_, matrix::room &room_);
  ~room();

  chat &get_chat() const;
  participants &get_participants() const;
  users &get_users() const;
  utils::signal::signal<const std::string &> on_name_changed;

  boost::optional<std::string> get_name() const;
  std::string get_own_id() const;
  boost::future<void> leave();

  matrix::room &get_native() const;

protected:
  client::logger logger;
  std::unique_ptr<participant_creator> participant_creator_;
  boost::inline_executor executor;
  std::shared_ptr<matrix::client> client_;
  matrix::room &room_;
  std::unique_ptr<chat> chat_;
  std::unique_ptr<users> users_;
  std::unique_ptr<participants> participants_;
};
} // namespace client

#endif
