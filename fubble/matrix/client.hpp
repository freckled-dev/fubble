#ifndef UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B
#define UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B

#include "factory.hpp"
#include "http/client_factory.hpp"
#include "rooms.hpp"
#include "users.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>

namespace matrix {
class client {
public:
  struct information {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  // TODO refactor to just taking a http::client?
  client(factory &factory_, http::client_factory &http_factory,
         const information &information_);
  ~client();

  const std::string &get_user_id() const;
  users &get_users() const;
  rooms &get_rooms() const;

  http::client &get_http_client();
  // TODO make protected
  std::unique_ptr<http::client> create_http_client();
  std::string create_transaction_id();

  boost::future<void> set_display_name(const std::string &name);
  boost::future<void> set_presence(presence set);
  // do not more than 50 seconds. because a minute is a magic numbers. eg
  // proxies will start to close the connection
  static constexpr std::chrono::milliseconds default_sync_timeout =
      std::chrono::seconds(30);
  boost::future<void>
  sync(std::chrono::milliseconds timeout = default_sync_timeout);
  boost::future<void> sync_till_empty_response();
  boost::future<void>
  sync_till_stop(std::chrono::milliseconds timeout = default_sync_timeout);
  void stop_sync();

  boost::signals2::signal<void(const nlohmann::json &)> on_sync;

protected:
  void on_synced(const nlohmann::json &content);
  std::string make_sync_target(std::chrono::milliseconds timeout) const;
  void on_sync_till_stop(
      boost::future<std::pair<boost::beast::http::status, nlohmann::json>>
          &response);
  void do_sync();

  matrix::logger logger;
  boost::inline_executor executor;
  factory &factory_;
  http::client_factory &http_factory;
  // TODO move all sync_till_stop logic to own class or at least collect the
  // fields in its own struct
  std::unique_ptr<boost::promise<void>> sync_till_stop_promise;
  std::unique_ptr<http::client> http_client;
  std::chrono::milliseconds sync_till_stop_timeout;
  bool sync_till_stop_active{};
  const information information_;
  boost::optional<std::string> sync_next_batch;
  std::unique_ptr<users> users_;
  std::unique_ptr<rooms> rooms_;
  int transaction_id_counter{};
  std::unique_ptr<http::action> http_sync_action;
};
} // namespace matrix

#endif
