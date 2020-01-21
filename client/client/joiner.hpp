#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "executor_asio.hpp"
#include "logging/logger.hpp"
#include <boost/asio/executor.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <boost/thread/future.hpp>

namespace session {
class client;
}

namespace client {
class joiner {
public:
  joiner(boost::asio::executor &executor);
  ~joiner();

  struct parameters {
    std::string name, room;
  };
  boost::future<void> join(const parameters &parameters_);

protected:
  logging::logger logger;
  boost::asio::executor &executor;
  executor_asio executor_asio_{executor};
  boost::executor_adaptor<executor_asio> future_executor{executor_asio_};
  class join;
};
} // namespace client

#endif
