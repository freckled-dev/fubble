#include "authentification.hpp"
#include "error.hpp"
#include <nlohmann/json.hpp>

using namespace matrix;

authentification::authentification(http::client_factory &http_client_creator,
                                   client_factory &client_factory_)
    : http_client_creator(http_client_creator),
      client_factory_(client_factory_) {
  http_client = http_client_creator.create();
}

boost::future<authentification::client_ptr>
authentification::register_as_guest() {
  const std::string target = "register?kind=guest";
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  auto result =
      http_client->post(target, register_).then(executor, [this](auto result) {
        auto response = result.get();
        if (response.first != boost::beast::http::status::ok)
          error::check_matrix_response(response.first, response.second);
        auto response_json = response.second;
        BOOST_LOG_SEV(this->logger, logging::severity::info)
            << "successfully registered as guest"
#if 0
            << ", response_json:" << response_json.dump(2)
#endif
            ;
        client::information information;
        information.access_token = response_json["access_token"];
        information.user_id = response_json["user_id"];
        information.device_id = response_json["device_id"];
        return client_factory_.create(information);
      });
  return result;
}

boost::future<authentification::client_ptr>
authentification::register_(const std::string &username,
                            const std::string &password) {
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  register_["username"] = username;
  register_["password"] = password;
  return register_as_user(register_);
}

boost::future<authentification::client_ptr>
authentification::register_anonymously() {
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  register_["password"] = "";
  return register_as_user(register_);
}

boost::future<authentification::client_ptr>
authentification::register_as_user(const nlohmann::json &content) {
  const std::string target = "register?kind=user";
  auto result =
      http_client->post(target, content).then(executor, [this](auto result) {
        auto response = result.get();
        if (response.first != boost::beast::http::status::ok)
          error::check_matrix_response(response.first, response.second);
        BOOST_LOG_SEV(this->logger, logging::severity::info)
            << "successfully registered as user";
        auto response_json = response.second;
        client::information information;
        information.access_token = response_json["access_token"];
        information.user_id = response_json["user_id"];
        information.device_id = response_json["device_id"];
        return client_factory_.create(information);
      });
  return result;
}
