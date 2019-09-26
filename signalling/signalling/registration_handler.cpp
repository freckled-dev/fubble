#include "registration_handler.hpp"
#include "connection.hpp"

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
  try {
    const registration registration_ = result.get();
  } catch (const boost::system::system_error &error) {
    boost::log::sources::severity_logger logger;
    BOOST_LOG_SEV(logger, logging::severity::warning) << "fun";
  }
}
