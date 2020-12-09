#ifndef UUID_341BB8DD_DF67_4277_ABD0_05A336C3EE40
#define UUID_341BB8DD_DF67_4277_ABD0_05A336C3EE40

#include "utils/export.hpp"
#include <memory>

namespace boost {
namespace asio {
class io_context;
}
namespace executors {
class executor;
}
} // namespace boost

namespace utils {
class timer_factory;
class FUBBLE_PUBLIC executor_module {
public:
  executor_module();
  virtual ~executor_module();

  virtual std::shared_ptr<boost::asio::io_context> get_io_context();
  virtual std::shared_ptr<boost::executors::executor> get_boost_executor();
  virtual std::shared_ptr<timer_factory> get_timer_factory();

protected:
  std::shared_ptr<boost::asio::io_context> io_context;
  std::shared_ptr<boost::executors::executor> boost_executor;
  std::shared_ptr<timer_factory> timer_factory_;
};
} // namespace utils

#endif
