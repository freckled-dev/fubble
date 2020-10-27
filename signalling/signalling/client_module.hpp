#ifndef UUID_E9815691_3E6C_4F4E_B1CE_482F98FD16A2
#define UUID_E9815691_3E6C_4F4E_B1CE_482F98FD16A2

#include "utils/executor_module.hpp"

namespace utils {
class one_shot_timer;
}
namespace websocket {
class connection_creator;
class connector_creator;
} // namespace websocket
namespace signalling::client {
class connection_creator;
class factory;
class factory_reconnecting;
struct connect_information;
} // namespace signalling::client
namespace signalling {
class json_message;
class client_module {
public:
  struct config {
    // TODO
  };

  client_module(std::shared_ptr<utils::executor_module> executor_module);
  virtual ~client_module();
  virtual client::connect_information get_connect_information();
  virtual std::shared_ptr<websocket::connector_creator>
  get_websocket_connector_creator();
  virtual std::shared_ptr<websocket::connection_creator>
  get_websocket_connection_creator();
  virtual std::shared_ptr<json_message> get_json_message_();
  virtual std::shared_ptr<client::connection_creator>
  get_client_connection_creator_();
  virtual std::shared_ptr<client::factory> get_client_creator();
  virtual std::shared_ptr<utils::one_shot_timer> get_reconnect_timer();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<websocket::connector_creator> websocket_connector_creator;
  std::shared_ptr<websocket::connection_creator> websocket_connection_creator;
  std::shared_ptr<json_message> json_message_;
  std::shared_ptr<client::connection_creator> client_connection_creator_;
  std::shared_ptr<client::factory> client_creator;
  std::shared_ptr<utils::one_shot_timer> reconnect_timer;
  std::shared_ptr<client::factory> client_creator_reconnecting;
};
} // namespace signalling

#endif
