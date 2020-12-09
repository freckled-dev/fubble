#ifndef UUID_1364323B_4F34_48AA_A740_45367F40E78D
#define UUID_1364323B_4F34_48AA_A740_45367F40E78D

#include "client.hpp"
#include "client_factory.hpp"
#include "fubble/http/client_factory.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class authentification {

public:
  authentification(http::client_factory &http_client_creator,
                   client_factory &client_factory_);
  struct login_result {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  using client_ptr = std::unique_ptr<client>;
  boost::future<client_ptr> register_as_guest();
  struct user_information {
    std::string username;
    std::string password;
    boost::optional<std::string> device_id;
  };
  boost::future<client_ptr> register_(const user_information &information);
  boost::future<client_ptr> register_anonymously();
  // https://matrix.org/docs/spec/client_server/latest#get-matrix-client-r0-login
  boost::future<client_ptr> login(const user_information &information);

protected:
  boost::future<client_ptr> register_as_user(const nlohmann::json &content);
  client_ptr on_registered(http::client::async_result_future &result);
  client_ptr on_logged_in(http::client::async_result_future &result);
  client_ptr
  make_client_from_response(http::client::async_result_future &result);

  matrix::logger logger{"authentification"};
  boost::inline_executor executor;
  http::client_factory &http_client_creator;
  client_factory &client_factory_;
  std::unique_ptr<http::client> http_client;
};
} // namespace matrix

#endif
