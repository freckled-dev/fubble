#include "creator.hpp"
#include "device.hpp"

using namespace signalling::device;

creator::creator(boost::executor &executor) : executor(executor) {}

device_ptr creator::create(connection_ptr connection_) {
  return std::make_unique<device>(connection_);
}

