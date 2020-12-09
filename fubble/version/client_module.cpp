#include "client_module.hpp"
#include "fubble/version/getter.hpp"

using namespace version;

client_module::client_module(
    std::shared_ptr<http::client_module> http_client_module,
    const config &config_)
    : http_client_module{http_client_module}, config_{config_} {}

std::shared_ptr<getter> client_module::get_getter() {
  if (!version_getter) {
    http::server http_version_client_server{config_.host, config_.service};
    http_version_client_server.secure = config_.use_ssl;
    http::fields http_version_client_fields{http_version_client_server};
    http_version_client_fields.target_prefix = config_.target;
    auto version_http_client = std::make_shared<http::client>(
        http_client_module->get_action_factory(),
        std::make_pair(http_version_client_server, http_version_client_fields));
    version_getter = getter::create(version_http_client);
  }
  return version_getter;
}
