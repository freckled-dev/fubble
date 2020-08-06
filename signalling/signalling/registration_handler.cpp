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
      "a device wants to register itself, key:'{}', reconnect_token:'{}'",
      registration_.key, registration_.reconnect_token);
  auto found = find(registration_.key);
  if (found == devices.cend()) {
    if (replace_connection_if_exists(connection_, found, registration_))
      return;
    register_as_first(connection_, registration_);
    return;
  }
  register_as_second(connection_, found, registration_);
}

bool registration_handler::replace_connection_if_exists(
    const connection_ptr &connection_, const devices_type::iterator &found,
    const registration &registration_) {
  BOOST_ASSERT(connection_);
  BOOST_ASSERT(found != devices.end());
  int index{};
  for (auto &device_ : found->devices) {
    if (device_->get_token() == registration_.reconnect_token) {
      auto old_device = device_;
      if (index == 0)
        register_as_first(connection_, registration_);
      else
        register_as_second(connection_, registration_);
      old_device->close();
      return true;
    }
    ++index;
  }
  return false;
}

void registration_handler::register_as_first(
    const connection_ptr &connection_, const registration &registration_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << fmt::format("a device wants to register itself as first, key:'{}'",
                     registration_.key);
  auto first_device =
      device_creator_.create(connection_, registration_.reconnect_token);
  auto connection_on_close = connection_->on_closed.connect(
      [this, registration_] { on_first_device_closed(registration_.key); });
  registered_connection device_;
  device_.key = registration_.key;
  device_.devices[0] = first_device;
  device_.on_close_handles[0] = std::move(connection_on_close);
  devices.push_back(std::move(device_));
}
void registration_handler::register_as_second(
    const connection_ptr &connection_, const devices_type::iterator &pair,
    const registration &registration_) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << fmt::format(
      "a device wants to register itself as answering, key:'{}'", pair->key);
  auto first_device = pair->devices.front();
  auto second_device =
      device_creator_.create(connection_, registration_.reconnect_token);
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
  auto &other_device = found->devices[1];
  if (other_device)
    other_device->reset_partner();
  on_device_closed(key);
}

void registration_handler::on_second_device_closed(const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("an answering device disconnected. key:'{}'", key);
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  auto &other_device = found->devices[0];
  if (other_device)
    other_device->reset_partner();
  on_device_closed(key);
}

void registration_handler::on_device_closed(const std::string &key) {
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  for (auto &device_ : found->devices)
    if (device_)
      return;
  // no device in pair no more - delete device
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
