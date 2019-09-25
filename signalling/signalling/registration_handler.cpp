#include "registration_handler.hpp"
#include "connection.hpp"

using namespace signalling;

registration_handler::registration_handler(device_creator &device_creator_)
    : device_creator_(device_creator_) {
  ;
}
void registration_handler::add(connection_ptr connection_) {
  auto result = connection_->read_registration();
  result.then([](boost::future<registration>) {});
}
