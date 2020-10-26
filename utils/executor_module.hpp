#ifndef UUID_341BB8DD_DF67_4277_ABD0_05A336C3EE40
#define UUID_341BB8DD_DF67_4277_ABD0_05A336C3EE40

#include <memory>

namespace boost::asio {
class io_context;
}
namespace boost::executors {
class executor;
}

namespace utils {
class executor_module {
public:
  executor_module();
  virtual ~executor_module();

  virtual std::shared_ptr<boost::asio::io_context> get_io_context();
  virtual std::shared_ptr<boost::executors::executor> get_boost_executor();

protected:
  std::shared_ptr<boost::asio::io_context> io_context;
  std::shared_ptr<boost::executors::executor> boost_executor;
};
} // namespace utils

#endif
