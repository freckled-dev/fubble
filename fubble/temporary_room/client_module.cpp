#include "client_module.hpp"
#include "http/client.hpp"
#include "temporary_room/net/client.hpp"

using namespace temporary_room;

client_module::client_module(
    std::shared_ptr<http::client_module> http_client_module,
    const config &config_)
    : http_client_module{http_client_module}, config_{config_} {}

client_module::~client_module() = default;

std::shared_ptr<temporary_room::net::client> client_module::get_client() {
  if (!client_)
    client_ = std::make_shared<temporary_room::net::client>(*get_http_client());
  return client_;
}

std::shared_ptr<http::client> client_module::get_http_client() {

  if (!http_client) {
    http::server http_temporary_room_client_server{config_.host,
                                                   config_.service};
    http_temporary_room_client_server.secure = config_.use_ssl;
    http::fields http_temporary_room_client_fields{
        http_temporary_room_client_server};
    http_temporary_room_client_fields.target_prefix = config_.target;
    http_client = std::make_shared<http::client>(
        http_client_module->get_action_factory(),
        std::make_pair(http_temporary_room_client_server,
                       http_temporary_room_client_fields));
  }
  return http_client;
}
