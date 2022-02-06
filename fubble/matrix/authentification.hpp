#ifndef UUID_1364323B_4F34_48AA_A740_45367F40E78D
#define UUID_1364323B_4F34_48AA_A740_45367F40E78D

#include <fubble/http2/module.hpp>
#include <fubble/matrix/client.hpp>
#include <fubble/matrix/client_factory.hpp>
#include <fubble/utils/lifetime.hpp>

namespace matrix {
class authentification {

public:
  authentification(
      const std::shared_ptr<fubble::http2::requester> &http_client_creator,
      client_factory &client_factory_);
  struct login_result {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  using client_ptr = std::unique_ptr<client>;
  client_ptr register_as_guest();
  struct user_information {
    std::string username;
    std::string password;
    boost::optional<std::string> device_id;
  };
  client_ptr register_(const user_information &information);
  client_ptr register_anonymously();
  // https://matrix.org/docs/spec/client_server/latest#get-matrix-client-r0-login
  client_ptr login(const user_information &information);

protected:
  client_ptr register_as_user(const nlohmann::json &content);
  client_ptr on_registered(fubble::http2::response_result &result);
  client_ptr on_logged_in(fubble::http2::response_result &result);
  client_ptr make_client_from_response(
      const fubble::http2::response_result &response_result);

  matrix::logger logger{"authentification"};
  const std::shared_ptr<fubble::http2::requester> http_requester;
  client_factory &client_factory_;
  fubble::lifetime::checker lifetime_checker;
};
} // namespace matrix

#endif
