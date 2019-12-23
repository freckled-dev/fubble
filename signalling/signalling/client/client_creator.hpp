#ifndef UUID_9E47829A_2415_4A40_88D6_4E7BC4A51037
#define UUID_9E47829A_2415_4A40_88D6_4E7BC4A51037

#include <boost/thread/executor.hpp>
#include <memory>

namespace websocket {
class connector;
}
namespace signalling::client {
class client;
class connection_creator;
class connection;
class client_creator {
public:
  client_creator(boost::executor &executor, websocket::connector &connector,
                 connection_creator &connection_creator_);

  std::unique_ptr<client> operator()();

protected:
  boost::executor &executor;
  websocket::connector &connector;
  connection_creator &connection_creator_;
};
} // namespace signalling::client

#endif
