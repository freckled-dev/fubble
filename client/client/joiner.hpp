#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "client/logger.hpp"
#include "executor_asio.hpp"
#include <boost/asio/executor.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <boost/thread/future.hpp>

namespace session {
class client;
}

namespace client {
class room;
class rooms;
class room_creator;
class joiner {
public:
  joiner(boost::asio::executor &executor, room_creator &room_creator_,
         rooms &rooms_);
  ~joiner();

  struct parameters {
    std::string name, room;
  };
  // thread-safe
  // TODO remove thread safety and executor from constructor
  boost::future<std::shared_ptr<room>> join(const parameters &parameters_);

protected:
  client::logger logger{"joiner"};
  boost::asio::executor &executor;
  room_creator &room_creator_;
  rooms &rooms_;
  executor_asio executor_asio_{executor};
  boost::executor_adaptor<executor_asio> future_executor{executor_asio_};
  class join;
};
} // namespace client

#endif
