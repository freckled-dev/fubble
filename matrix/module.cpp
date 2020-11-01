#include "module.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/factory.hpp"
#include "matrix/rooms.hpp"

using namespace matrix;

module::module(std::shared_ptr<utils::executor_module> executor_module,
               std::shared_ptr<http::client_module> http_module,
               const config &config_)
    : executor_module{executor_module},
      http_module{http_module}, config_{config_} {}

module::~module() = default;

std::shared_ptr<factory> module::get_factory() {
  if (!factory_)
    factory_ = std::make_shared<factory>();
  return factory_;
}

std::shared_ptr<client_factory> module::get_client_factory() {
  if (!client_factory_)
    client_factory_ = std::make_shared<client_factory>(
        *get_factory(), *get_http_client_factory());
  return client_factory_;
}

std::shared_ptr<authentification> module::get_authentification() {
  if (!authentification_)
    authentification_ = std::make_shared<authentification>(
        *get_http_client_factory(), *get_client_factory());
  return authentification_;
}

std::shared_ptr<http::client_factory> module::get_http_client_factory() {
  if (!http_client_factory) {
    http::server http_matrix_client_server{config_.host, config_.service};
    http_matrix_client_server.secure = config_.use_ssl;
    http::fields http_matrix_client_fields{http_matrix_client_server};
    http_matrix_client_fields.target_prefix = config_.target;
    http_client_factory = std::make_shared<http::client_factory>(
        http_module->get_action_factory(),
        std::make_pair(http_matrix_client_server, http_matrix_client_fields));
  }
  return http_client_factory;
}
