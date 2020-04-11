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
  boost::signals2::signal<void(std::vector<participant *>)> on_joins;
  boost::signals2::signal<void(std::vector<std::string>)> on_leaves;
  boost::signals2::signal<void(std::string)> on_name_changed;
  std::vector<participant *> get_participants() const;
  std::string get_name() const;
  // TODO getter --> `get_`
  std::string get_id() const;

protected:
  void on_join(matrix::user &user);
  void on_leave(const std::string &user_id);
  using participants = std::vector<std::unique_ptr<participant>>;
  participants::iterator find_iterator(const std::string &user_id);

  session::logger logger{"room"};
  matrix::room &room_;
  participants participants_;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace session

#endif
