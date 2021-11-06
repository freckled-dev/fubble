#ifndef SIGNALING_REGISTRATION_HANDLER_HPP
#define SIGNALING_REGISTRATION_HANDLER_HPP

#include "connection_ptr.hpp"
#include "logger.hpp"
#include "fubble/signaling/device/device_ptr.hpp"
#include <sigslot/signal.hpp>
#include <boost/thread/executors/executor.hpp>
#include <boost/thread/future.hpp>
#include <utility>
#include <vector>

namespace signaling {
namespace device {
class creator;
} // namespace device
struct registration;
class registration_handler {
public:
  registration_handler(device::creator &device_creator_);

  void add(connection_ptr connection_);
  struct registered_connection {
    std::string key;
    std::array<device::device_ptr, 2> devices;
    std::array<sigslot::scoped_connection, 2> on_close_handles;
  };
  using devices_type = std::vector<registered_connection>;
  const devices_type &get_registered() const;

private:
  void on_register(const connection_ptr &connection_, const registration &);
  void register_(const connection_ptr &connection_, const registration &);
  void on_device_closed(const std::string &key,
                        const std::string &registration_token);
  void remove_by_key(const std::string &key);
  devices_type::iterator find(const std::string &key);
  registered_connection &find_or_create(const std::string &key);

  signaling::logger logger{"registration_handler"};
  device::creator &device_creator_;
  devices_type devices;
};
} // namespace signaling

#endif
