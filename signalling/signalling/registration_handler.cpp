#include "registration_handler.hpp"
#include "connection.hpp"
#include <fmt/format.h>

using namespace signalling;

registration_handler::registration_handler(device::creator &device_creator_)
    : device_creator_(device_creator_) {}
void registration_handler::add(connection_ptr connection_) {
  auto result = connection_->read_registration();
  result.then(
      [this, connection_](auto result) { on_register(connection_, result); });
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
                                     const registration &registraion_) {
  auto found =
      std::find_if(devices.begin(), devices.end(), [&](const auto &check) {
        return check.key == registraion_.key;
      });
  if (found == devices.cend()) {
    register_as_offering(connection_);
    return;
  }
  register_as_answering(connection_);
}

void registration_handler::register_as_offering(
    const connection_ptr &connection_) {
  ;
}
void registration_handler::register_as_answering(
    const connection_ptr &connection_) {
  ;
}
