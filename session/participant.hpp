#ifndef UUID_664721D3_17CF_427C_BB2A_BE79190FEB7D
#define UUID_664721D3_17CF_427C_BB2A_BE79190FEB7D

#include "matrix/user.hpp"
#include <string>

namespace session {
class participant {
public:
  participant(matrix::user &matrix_user) : matrix_user(matrix_user) {
    connection_update = matrix_user.on_update.connect([this] { on_update(); });
  }

  boost::signals2::signal<void()> on_update;

  inline std::string get_id() const { return matrix_user.get_id(); }
  inline std::string get_name() const { return matrix_user.get_display_name(); }

protected:
  matrix::user &matrix_user;
  boost::signals2::scoped_connection connection_update;
};
} // namespace session

#endif
