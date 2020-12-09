#include "creator.hpp"
#include "device.hpp"

using namespace signaling::device;

creator::creator(boost::executor &executor) : executor(executor) {}

device_ptr creator::create(signaling::connection_ptr connection_,
                           const std::string &token) {
  return std::make_unique<device>(connection_, token);
}
