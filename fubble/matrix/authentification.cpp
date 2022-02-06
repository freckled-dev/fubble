#include "authentification.hpp"
#include "error.hpp"
#include <fubble/utils/uuid.hpp>
#include <nlohmann/json.hpp>

using namespace matrix;

authentification::authentification(
    const std::shared_ptr<fubble::http2::requester> &http_requester,
    client_factory &client_factory_)
    : http_requester(http_requester), client_factory_(client_factory_) {}

boost::future<authentification::client_ptr>
authentification::register_as_guest() {
  const std::string target = "register?kind=guest";
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;

  auto post_request = http_requester->post(target, register_);

  auto promise =
      std::make_shared<boost::promise<authentification::client_ptr>>();
  auto result = promise->get_future();
  post_request->async_run(
      lifetime_checker.wrap<const fubble::http2::response_result &>(
          [this, promise](const auto &response) {
            try {
              promise->set_value(make_client_from_response(response));
            } catch (...) {
              promise->set_exception(boost::current_exception());
            }
          }));
  return result;
}

boost::future<authentification::client_ptr>
authentification::register_(const user_information &information) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  register_["username"] = information.username;
  register_["password"] = information.password;
  if (information.device_id)
    register_["device_id"] = information.device_id.value();
  return register_as_user(register_);
}

boost::future<authentification::client_ptr>
authentification::register_anonymously() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto register_ = nlohmann::json::object();
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  register_["password"] = uuid::generate();
  return register_as_user(register_);
}

boost::future<authentification::client_ptr>
authentification::login(const user_information &information) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto login_ = nlohmann::json::object();
  login_["type"] = "m.login.password";
  login_["user"] = information.username;
  login_["password"] = information.password;
  if (information.device_id)
    login_["device_id"] = information.device_id.value();
  {
    auto identifier = nlohmann::json::object();
    identifier["type"] = "m.id.user";
    login_["identifier"] = identifier;
  }
  const std::string target = "login";

  auto post_request = http_requester->post(target, login_);

  auto promise =
      std::make_shared<boost::promise<authentification::client_ptr>>();
  auto result = promise->get_future();
  post_request->async_run(
      lifetime_checker.wrap<const fubble::http2::response_result &>(
          [this, promise](const auto &response) {
            try {
              promise->set_value(on_logged_in(response));
            } catch (...) {
              promise->set_exception(boost::current_exception());
            }
          }));
  return result;
}

boost::future<authentification::client_ptr>
authentification::register_as_user(const nlohmann::json &content) {
  const std::string target = "register?kind=user";

  auto post_request = http_requester->post(target, content);

  auto promise =
      std::make_shared<boost::promise<authentification::client_ptr>>();
  auto result = promise->get_future();

  post_request->async_run(
      lifetime_checker.wrap<const fubble::http2::response_result &>(
          [this, promise](const auto &response) {
            try {
              promise->set_value(on_registered(response));
            } catch (...) {
              promise->set_exception(boost::current_exception());
            }
          }));

  return result;
}

authentification::client_ptr
authentification::on_registered(const fubble::http2::response_result &result) {
  return make_client_from_response(result);
}

authentification::client_ptr
authentification::on_logged_in(const fubble::http2::response_result &result) {
  return make_client_from_response(result);
}

authentification::client_ptr authentification::make_client_from_response(
    const fubble::http2::response_result &response_result) {
  const auto &response = response_result.value();
  if (response.code != 200)
    error::check_matrix_response(response.code, *response.body);
  const auto &response_json = *response.body;
  BOOST_LOG_SEV(this->logger, logging::severity::info)
      << "successfully registered as user or guest";
  client::information information;
  information.access_token = response_json["access_token"];
  information.user_id = response_json["user_id"];
  information.device_id = response_json["device_id"];
  return client_factory_.create(information);
}
