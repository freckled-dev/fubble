#ifndef UUID_2D23136F_1A42_4FB0_AE5C_21F235BC7591
#define UUID_2D23136F_1A42_4FB0_AE5C_21F235BC7591

#include <boost/signals2/signal.hpp>

namespace matrix {
class users;
}

namespace client {
struct user {
  std::string id;
  std::string name;
};
class users {
public:
  users(matrix::users &matrix_users);
#if 0 // TODO implement
  boost::signals2::signal<void(user &)> on_added;
#endif

  user get_by_id(const std::string &id) const;

protected:
  matrix::users &matrix_users;
};
} // namespace client

#endif
