#include "registration_handler.hpp"
#include "signalling/connection.hpp"
#include "signalling/device/creator.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

struct RegistrationHandler : testing::Test {
  signalling::device::creator creator;
  boost::inline_executor executor;
  boost::generic_executor_ref executor_ref{executor};
  signalling::registration_handler handler{executor_ref, creator};
};

struct mock_connection : signalling::connection {
  boost::future<signalling::registration> read_registration() override {
    signalling::registration result;
    result.key = "first";
    return boost::make_ready_future(result);
  }
};

TEST_F(RegistrationHandler, Setup) {
  EXPECT_TRUE(handler.get_registered().empty());
}

static signalling::connection_ptr
add_connection(signalling::registration_handler &handler) {
  auto connection = std::make_shared<mock_connection>();
  handler.add(connection);
  return connection;
}

TEST_F(RegistrationHandler, AddOffering) {
  add_connection(handler);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
}

TEST_F(RegistrationHandler, AddOfferAnswer) {
  add_connection(handler);
  add_connection(handler);
  EXPECT_TRUE(handler.get_registered().empty());
}

