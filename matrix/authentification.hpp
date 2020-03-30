#ifndef UUID_1364323B_4F34_48AA_A740_45367F40E78D
#define UUID_1364323B_4F34_48AA_A740_45367F40E78D

#include "client.hpp"
#include "client_factory.hpp"
#include "http/client_factory.hpp"
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
  boost::future<client_ptr> register_(const std::string &username,
                                      const std::string &password);
  boost::future<client_ptr> register_anonymously();
  boost::future<client_ptr> login(const std::string &username,
                                  const std::string &password);

protected:
  boost::future<client_ptr> register_as_user(const nlohmann::json &content);

  matrix::logger logger{"authentification"};
  boost::inline_executor executor;
  http::client_factory &http_client_creator;
  client_factory &client_factory_;
  std::unique_ptr<http::client> http_client;
};
} // namespace matrix

#endif
