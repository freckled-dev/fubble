#ifndef UUID_1EC5D6A5_A3A5_44B8_9B59_3A70304E057E
#define UUID_1EC5D6A5_A3A5_44B8_9B59_3A70304E057E

#include "client.hpp"
#include <boost/thread/executor.hpp>
#include <memory>

namespace websocket {
class connector;
}
namespace signalling::client {
class connection_creator;
class connection;
class client_creator {
public:
  client_creator(websocket::connector_creator &connector_creator,
                 connection_creator &connection_creator_,
                 const client::connect_information &connect_information_);

  std::unique_ptr<client> operator()();

protected:
  websocket::connector_creator &connector_creator;
  connection_creator &connection_creator_;
  const client::connect_information connect_information_;
};
} // namespace signalling::client

#endif
