#ifndef SIGNALLING_REGISTRATION_HANDLER_HPP
#define SIGNALLING_REGISTRATION_HANDLER_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include "signalling/device/answering_ptr.hpp"
#include "signalling/device/offering_ptr.hpp"
#include <boost/signals2/connection.hpp>
#include <boost/thread/executors/executor.hpp>
#include <boost/thread/future.hpp>
#include <vector>

namespace signalling {
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
    device::offering_ptr offering_device;
    device::answering_ptr answering_device;
    std::array<boost::signals2::scoped_connection, 2> on_close_handles;
  };
  using devices_type = std::vector<registered_connection>;
  const devices_type &get_registered() const;

private:
  void on_register(const connection_ptr &connection_, const registration &);
  void register_(const connection_ptr &connection_, const registration &);
  void register_as_offering(const connection_ptr &connection_,
                            const std::string &key);
  void register_as_answering(const connection_ptr &connection_,
                             const devices_type::iterator &offering);
  void on_offering_device_closed(const std::string &key);
  void on_answering_device_closed(const std::string &key);
  void on_device_closed(const std::string &key);
  void remove_by_key(const std::string &key);
  devices_type::iterator find(const std::string &key);

  logging::logger logger;
  device::creator &device_creator_;
  devices_type devices;
};
} // namespace signalling

#endif
