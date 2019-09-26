#ifndef SIGNALLING_REGISTRATION_HANDLER_HPP
#define SIGNALLING_REGISTRATION_HANDLER_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include <boost/thread/future.hpp>
#include <memory>
#include <vector>

namespace signalling {
namespace device {
class creator;
}
struct registration;
class registration_handler {
public:
  registration_handler(device::creator &device_creator_);

  void add(connection_ptr connection_);

private:
  void on_register(const connection_ptr &connection_,
                   boost::future<registration> &);

  logging::logger logger;
  device::creator &device_creator_;
  struct registered_device {
    std::string key;
    connection_ptr device;
  };
  std::vector<registered_device> devices;
};
} // namespace signalling

#endif
