#include "registration_handler.hpp"
#include "signalling/device/creator.hpp"
#include <gtest/gtest.h>

TEST(RegistrationHandler, Setup) {
  signalling::device::creator creator;
  signalling::registration_handler handler{creator};
}
