#include "authentification.hpp"
#include "error.hpp"

using namespace matrix;

authentification::authentification(http::client &http_client)
    : http_client(http_client) {}

boost::future<authentification::login_result>
authentification::register_as_guest() {
  const std::string target = "register?kind=guest";
  auto register_ = nlohmann::json::object();
  // register_["initial_device_display_name"] = "A fun guest name";
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  auto result =
      http_client.post(target, register_).then(executor, [](auto result) {
        auto response = result.get();
        if (response.first != boost::beast::http::status::ok)
          error::check_matrix_response(response.first, response.second);
        auto response_json = response.second;
        login_result login_result_;
        login_result_.access_token = response_json["access_token"];
        login_result_.user_id = response_json["user_id"];
        login_result_.device_id = response_json["device_id"];
        return login_result_;
      });
  return result;
}

boost::future<authentification::login_result>
authentification::register_(const std::string &username,
                            const std::string &password) {
  const std::string target = "register?kind=user";
  auto register_ = nlohmann::json::object();
  // register_["initial_device_display_name"] = "A fun guest name";
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  register_["username"] = username;
  register_["password"] = password;
  auto result =
      http_client.post(target, register_).then(executor, [](auto result) {
        auto response = result.get();
        if (response.first != boost::beast::http::status::ok)
          error::check_matrix_response(response.first, response.second);
        auto response_json = response.second;
        login_result login_result_;
        login_result_.access_token = response_json["access_token"];
        login_result_.user_id = response_json["user_id"];
        login_result_.device_id = response_json["device_id"];
        return login_result_;
      });
  return result;
}
