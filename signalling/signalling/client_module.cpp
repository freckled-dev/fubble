#include "client_module.hpp"
#include "signalling/client/client.hpp"
#include "signalling/json_message.hpp"
#include "utils/timer.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <boost/asio/io_context.hpp>

using namespace signalling;

client_module::client_module(
    std::shared_ptr<utils::executor_module> executor_module)
    : executor_module(executor_module) {}

client_module::~client_module() = default;

client::connect_information client_module::get_connect_information() {
  signalling::client::connect_information result; /*{
      config.general_.use_ssl, config.general_.host, config.general_.service,
      "/api/signalling/v0/"};*/
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
        std::make_shared<signalling::client::connection_creator>(
            *executor_module->get_io_context(),
            *executor_module->get_boost_executor(), *get_json_message_());
  return client_connection_creator_;
}

std::shared_ptr<client::factory> client_module::get_client_creator() {
  if (!client_creator) {
    client_creator = std::make_shared<signalling::client::factory_impl>(
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
    std::make_shared<utils::one_shot_timer>(*executor_module->get_io_context(),
                                            std::chrono::seconds(1));
  return reconnect_timer;
}
