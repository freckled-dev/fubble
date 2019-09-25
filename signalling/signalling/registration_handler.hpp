#ifndef SIGNALLING_REGISTRATION_HANDLER_HPP
#define SIGNALLING_REGISTRATION_HANDLER_HPP

#include <boost/thread/future.hpp>
#include <memory>
#include <vector>

namespace signalling {
class connection;
class device_creator;
struct registration;
class registration_handler {
public:
  registration_handler(device_creator &device_creator_);

  using connection_ptr = std::shared_ptr<connection>;
  void add(connection_ptr connection_);

private:
  void on_register(const boost::future<registration> &);

  device_creator &device_creator_;
  struct registered_device {
    std::string key;
    connection_ptr device;
  };
  std::vector<registered_device> devices;
};
} // namespace signalling

#endif
