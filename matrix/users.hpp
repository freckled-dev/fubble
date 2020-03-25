#ifndef UUID_9043A9BB_1B7B_4DCA_AD6D_22FCC51B432D
#define UUID_9043A9BB_1B7B_4DCA_AD6D_22FCC51B432D

#include "client.hpp"
#include "user.hpp"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <deque>

namespace matrix {
class users {
public:
  users(client &client_);
  user &get_or_add_user(const std::string &id);

  boost::signals2::signal<void(client &)> on_add;

protected:
  void on_sync(const nlohmann::json &content);

  client &client_;
  boost::signals2::scoped_connection on_sync_connection;
  std::deque<std::unique_ptr<user>> users_;
};
} // namespace matrix

#endif
