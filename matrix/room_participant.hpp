#ifndef UUID_9A5F9DB4_A585_471D_9A8C_641DE3A54DF9
#define UUID_9A5F9DB4_A585_471D_9A8C_641DE3A54DF9

#include <boost/signals2/signal.hpp>

namespace matrix {
class user;
enum class join_state { join, leave, invite };
class room_participant {
public:
  room_participant(user &user_, const join_state join_state_);

  user &get_user() const;
  join_state get_join_state() const;
  void set_join_state(const join_state new_);
  std::string get_id() const;

  boost::signals2::signal<void()> on_join_state_changed;

protected:
  user &user_;
  join_state join_state_;
};
} // namespace matrix

#endif
