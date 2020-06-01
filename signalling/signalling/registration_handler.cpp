#include "registration_handler.hpp"
#include "connection.hpp"
#include "signalling/device/creator.hpp"
#include "signalling/device/device.hpp"
#include <fmt/format.h>

using namespace signalling;

registration_handler::registration_handler(device::creator &device_creator_)
    : device_creator_(device_creator_) {}

void registration_handler::add(connection_ptr connection_) {
  connection_->on_registration.connect(
      [this, connection_ = std::weak_ptr(connection_)](auto result) {
        on_register(connection_.lock(), result);
      });
}

const registration_handler::devices_type &
registration_handler::get_registered() const {
  return devices;
}

void registration_handler::on_register(const connection_ptr &connection_,
                                       const registration &registration_) {
  BOOST_ASSERT(connection_);
  try {
    register_(connection_, registration_);
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << fmt::format("an error occured, while trying to get the register "
                       "result. Description: '{}'",
                       error.what());
  }
}

void registration_handler::register_(const connection_ptr &connection_,
                                     const registration &registration_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "a device wants to register itself, key:'{}'", registration_.key);
  auto found = find(registration_.key);
  if (found == devices.cend()) {
    register_as_first(connection_, registration_.key);
    return;
  }
  register_as_second(connection_, found);
}

void registration_handler::register_as_first(const connection_ptr &connection_,
                                             const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << fmt::format(
      "a device wants to register itself as first, key:'{}'", key);
  auto first_device = device_creator_.create(connection_);
  auto connection_on_close = connection_->on_closed.connect(
      [this, key] { on_first_device_closed(key); });
  registered_connection device_;
  device_.key = key;
  device_.devices[0] = first_device;
  device_.on_close_handles[0] = std::move(connection_on_close);
  devices.push_back(std::move(device_));
}
void registration_handler::register_as_second(
    const connection_ptr &connection_, const devices_type::iterator &pair) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << fmt::format(
      "a device wants to register itself as answering, key:'{}'", pair->key);
  auto first_device = pair->devices.front();
  auto second_device = device_creator_.create(connection_);
  pair->devices[1] = second_device;
  auto connection_on_close = connection_->on_closed.connect(
      [this, key = pair->key] { on_second_device_closed(key); });
  pair->on_close_handles[1] = std::move(connection_on_close);
  first_device->set_partner(second_device);
  second_device->set_partner(first_device);
}
void registration_handler::on_first_device_closed(const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("an first device disconnected. key:'{}'", key);
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  if (found->devices[1])
    found->devices[1]->close();
  on_device_closed(key);
}

void registration_handler::on_second_device_closed(const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("an answering device disconnected. key:'{}'", key);
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  if (found->devices[0])
    found->devices[0]->close();
  on_device_closed(key);
}

void registration_handler::on_device_closed(const std::string &key) {
  remove_by_key(key);
}

void registration_handler::remove_by_key(const std::string &key) {
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  devices.erase(found);
}
registration_handler::devices_type::iterator
registration_handler::find(const std::string &key) {
  return std::find_if(devices.begin(), devices.end(),
                      [&](const auto &check) { return check.key == key; });
}
