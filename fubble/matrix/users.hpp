#ifndef UUID_9043A9BB_1B7B_4DCA_AD6D_22FCC51B432D
#define UUID_9043A9BB_1B7B_4DCA_AD6D_22FCC51B432D

#include "logger.hpp"
#include "user.hpp"
#include <fubble/utils/signal.hpp>
#include <deque>
#include <nlohmann/json_fwd.hpp>

namespace matrix {
class client;
class users {
public:
  users(client &client_);
  user &get_or_add_user(const std::string &id);
  user &get_by_id(const std::string &id);

  using user_list = std::deque<std::unique_ptr<user>>;
  inline const user_list &get_all() const { return users_; }
  utils::signal::signal<client &> on_add;

protected:
  void on_sync(const nlohmann::json &content);
  std::deque<std::unique_ptr<user>>::iterator find(const std::string &id);

  matrix::logger logger{"users"};
  client &client_;
  utils::signal::scoped_connection on_sync_connection;
  std::deque<std::unique_ptr<user>> users_;
};
} // namespace matrix

#endif
