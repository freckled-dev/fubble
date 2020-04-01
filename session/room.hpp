#ifndef UUID_5D5307B7_976F_4300_8388_7400A858C71C
#define UUID_5D5307B7_976F_4300_8388_7400A858C71C

#include "participant.hpp"
#include "session/logger.hpp"
#include <boost/signals2/signal.hpp>

namespace matrix {
class room;
class user;
} // namespace matrix

namespace session {
class client;
class room {
public:
  room(matrix::room &room_);
  using participants = std::vector<participant>;
  boost::signals2::signal<void(participants)> on_joins;
  boost::signals2::signal<void(std::vector<std::string>)> on_leaves;
  boost::signals2::signal<void(participants)> on_updates;
  const participants &get_participants() const;
  std::string get_name() const;
  // TODO getter --> `get_`
  std::string own_id() const;

protected:
  void on_join(const matrix::user &user);
  void on_leave(const std::string &user_id);
  participants::iterator find_iterator(const std::string &user_id);

  session::logger logger{"room"};
  matrix::room &room_;
  participants participants_;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace session

#endif
