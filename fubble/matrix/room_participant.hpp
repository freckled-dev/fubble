#ifndef UUID_9A5F9DB4_A585_471D_9A8C_641DE3A54DF9
#define UUID_9A5F9DB4_A585_471D_9A8C_641DE3A54DF9

#include "logger.hpp"
#include <fubble/utils/signal.hpp>
#include <chrono>

namespace matrix {
class user;
enum class join_state { join, leave, invite };
std::ostream &operator<<(std::ostream &out, const join_state print);
class room_participant {
public:
  using timestamp_type = std::chrono::system_clock::time_point;
  room_participant(user &user_, const join_state join_state_,
                   const timestamp_type &join_state_timestamp);

  user &get_user() const;
  join_state get_join_state() const;
  void set_join_state(const join_state new_, const timestamp_type &timestamp);
  std::string get_id() const;

  utils::signal::signal<> on_join_state_changed;

protected:
  user &user_;
  matrix::logger logger;
  timestamp_type join_state_timestamp;
  join_state join_state_;
};
} // namespace matrix

#endif
