#ifndef UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B
#define UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B

#include "factory.hpp"
#include "http/client_factory.hpp"
#include "users.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>

namespace matrix {
class users;
class client {
public:
  struct information {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  client(factory &factory_, http::client_factory &http_factory,
         const information &information_);

  const std::string &get_user_id() const;
  users &get_users() const;

  std::unique_ptr<http::client> create_http_client();

  boost::future<std::unique_ptr<room>> create_room();
  void leave_room(const room &room_);
  boost::future<std::unique_ptr<room>> join_room_by_id(const std::string &id);

  void set_display_name();
  boost::future<void>
  sync(std::chrono::milliseconds timeout = std::chrono::seconds(60));

  boost::signals2::signal<void(const nlohmann::json &)> on_sync;

protected:
  void on_synced(const nlohmann::json &content);

  matrix::logger logger{"client"};
  factory &factory_;
  http::client_factory &http_factory;
  std::unique_ptr<http::client> http_client;
  boost::inline_executor executor;
  const information information_;
  std::optional<std::string> sync_next_batch;
  std::unique_ptr<users> users_;
};
} // namespace matrix

#endif
