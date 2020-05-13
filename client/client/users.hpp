#ifndef UUID_2D23136F_1A42_4FB0_AE5C_21F235BC7591
#define UUID_2D23136F_1A42_4FB0_AE5C_21F235BC7591

#include <boost/signals2/signal.hpp>

namespace matrix {
class users;
}

namespace client {
class user;
class users {
public:
  users(matrix::users &matrix_users);
  boost::signals2::signal<void(user &)> on_added;

protected:
  matrix::users &matrix_users;
};
} // namespace client

#endif
