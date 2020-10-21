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
  registered_connection &change = find_or_create(registration_.key);
  auto first_device = change.devices[0];
  auto second_device = change.devices[1];
  boost::optional<std::size_t> my_device_index;
  if (first_device)
    if (first_device->get_token() == registration_.reconnect_token)
      my_device_index = 0;
  if (second_device)
    if (second_device->get_token() == registration_.reconnect_token)
      my_device_index = 1;
  if (!my_device_index) {
    BOOST_ASSERT(!first_device || !second_device);
    my_device_index = first_device == nullptr ? 0 : 1;
  }
  BOOST_ASSERT(my_device_index);
  auto my_device_index_value = my_device_index.value();
  std::size_t other_device_index = my_device_index_value == 0 ? 1 : 0;
  auto other_device = change.devices[other_device_index];
  auto device_ =
      device_creator_.create(connection_, registration_.reconnect_token);
  auto connection_on_close =
      connection_->on_closed.connect([this, registration_] {
        on_device_closed(registration_.key, registration_.reconnect_token);
      });
  if (change.devices[my_device_index_value]) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << __FUNCTION__ << "overwriting existing connection";
    change.devices[my_device_index_value]->close();
  }
  change.devices[my_device_index_value] = device_;
  change.on_close_handles[my_device_index_value] =
      std::move(connection_on_close);
  if (other_device) {
    BOOST_ASSERT(other_device->get_token() != device_->get_token());
    device_->set_partner(other_device);
    other_device->set_partner(device_);
  }
}

void registration_handler::on_device_closed(
    const std::string &key, const std::string &registration_token) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("an first device disconnected. key:'{}'", key);
  auto found = find(key);
  BOOST_ASSERT(found != devices.cend());
  const std::size_t remove_index = [&] {
    for (std::size_t index{}; index < found->devices.size(); ++index)
      if (found->devices[index])
        if (found->devices[index]->get_token() == registration_token)
          return index;
    BOOST_ASSERT(false);
    return std::size_t{};
  }();
  auto &other_device = found->devices[remove_index == 0 ? 1 : 0];
  if (other_device)
    other_device->reset_partner();
  found->on_close_handles[remove_index].disconnect();
  found->devices[remove_index].reset();
  for (auto &device_ : found->devices)
    if (device_)
      return;
  // no device in pair no more - delete device
  devices.erase(found);
}

registration_handler::devices_type::iterator
registration_handler::find(const std::string &key) {
  return std::find_if(devices.begin(), devices.end(),
                      [&](const auto &check) { return check.key == key; });
}

registration_handler::registered_connection &
registration_handler::find_or_create(const std::string &key) {
  auto found = find(key);
  if (found != devices.cend())
    return *found;
  registered_connection result;
  result.key = key;
  return devices.emplace_back(std::move(result));
}
