#include "registration_handler.hpp"
#include "connection.hpp"
#include "signalling/device/creator.hpp"
#include "signalling/device/offering.hpp"
#include <fmt/format.h>

using namespace signalling;

registration_handler::registration_handler(device::creator &device_creator_)
    : device_creator_(device_creator_) {}

void registration_handler::add(connection_ptr connection_) {
  auto result = connection_->read_registration();
  result.then(
      [this, connection_](auto result) { on_register(connection_, result); });
}

const registration_handler::devices_type &
registration_handler::get_registered() const {
  return devices;
}

void registration_handler::on_register(const connection_ptr &connection_,
                                       boost::future<registration> &result) {
  BOOST_ASSERT(connection_);
  try {
    const registration registration_ = result.get();
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
    register_as_offering(connection_, registration_.key);
    return;
  }
  register_as_answering(connection_, found);
}

void registration_handler::register_as_offering(
    const connection_ptr &connection_, const std::string &key) {
  auto offering_device = device_creator_.create_offering(connection_);
  auto connection_on_close = offering_device->on_closed.connect([] {});
  registered_device device_{key, offering_device,
                            std::move(connection_on_close)};
  devices.push_back(std::move(device_));
}
void registration_handler::register_as_answering(
    const connection_ptr &connection_, const devices_type::iterator &offering) {
  auto answering_device = device_creator_.create_answering(connection_);
  auto offering_device = offering->device;
  offering_device->set_partner(answering_device);
  devices.erase(offering);
}
void registration_handler::on_offering_device_closed(const std::string &key) {
  auto found = find(key);
  assert(found != devices.cend());
  devices.erase(found);
}

registration_handler::devices_type::iterator
registration_handler::find(const std::string &key) {
  return std::find_if(devices.begin(), devices.end(),
                      [&](const auto &check) { return check.key == key; });
}
