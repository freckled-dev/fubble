#include "client_module.hpp"
#include "signaling/client/client.hpp"
#include "signaling/json_message.hpp"
#include "utils/timer.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <boost/asio/io_context.hpp>

using namespace signaling;

client_module::client_module(
    std::shared_ptr<utils::executor_module> executor_module,
    const config &config_)
    : executor_module(executor_module), config_{config_} {}

client_module::~client_module() = default;

client::connect_information client_module::get_connect_information() {
  signaling::client::connect_information result;
  result.host = config_.host;
  result.secure = config_.secure;
  result.service = config_.service;
  result.target = config_.target;
  return result;
}

std::shared_ptr<websocket::connector_creator>
client_module::get_websocket_connector_creator() {
  if (!websocket_connector_creator)
    websocket_connector_creator =
        std::make_shared<websocket::connector_creator>(
            *executor_module->get_io_context(),
            *get_websocket_connection_creator());
  return websocket_connector_creator;
}

std::shared_ptr<websocket::connection_creator>
client_module::get_websocket_connection_creator() {
  if (!websocket_connection_creator)
    websocket_connection_creator =
        std::make_shared<websocket::connection_creator>(
            *executor_module->get_io_context());
  return websocket_connection_creator;
}

std::shared_ptr<json_message> client_module::get_json_message_() {
  if (!json_message_)
    json_message_ = std::make_shared<json_message>();
  return json_message_;
}

std::shared_ptr<client::connection_creator>
client_module::get_client_connection_creator_() {
  if (!client_connection_creator_)
    client_connection_creator_ =
        std::make_shared<signaling::client::connection_creator>(
            *executor_module->get_io_context(),
            *executor_module->get_boost_executor(), *get_json_message_());
  return client_connection_creator_;
}

std::shared_ptr<client::factory> client_module::get_client_creator() {
  if (!client_creator) {
    client_creator = std::make_shared<signaling::client::factory_impl>(
        *get_websocket_connector_creator(), *get_client_connection_creator_(),
        get_connect_information());
    client_creator_reconnecting =
        std::make_shared<client::factory_reconnecting>(*client_creator,
                                                       *get_reconnect_timer());
  }
  return client_creator_reconnecting;
}

std::shared_ptr<utils::one_shot_timer> client_module::get_reconnect_timer() {
  if (!reconnect_timer)
    reconnect_timer = std::make_shared<utils::one_shot_timer>(
        *executor_module->get_io_context(), config_.reconnect_timeout);
  return reconnect_timer;
}
