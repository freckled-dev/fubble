#include "registration_handler.hpp"
#include "signalling/connection.hpp"
#include "signalling/device/creator.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

struct RegistrationHandler : testing::Test {
  boost::inline_executor executor;
  boost::generic_executor_ref executor_ref{executor};
  signalling::device::creator creator{executor_ref};
  signalling::registration_handler handler{executor_ref, creator};
};

struct mock_connection : signalling::connection {
  ~mock_connection() final { on_closed(); }
  std::optional<signalling::offer> offer;
  void send_offer(const signalling::offer &send) final {
    EXPECT_FALSE(offer);
    offer = send;
  }
  void send_ice_candidate(const signalling::ice_candidate &candidate) final {}
  void send_answer() final {}
  bool state_offering_called{};
  void send_state_offering() final { state_offering_called = true; }
  bool state_answering_called{};
  void send_state_answering() final { state_answering_called = true; }
};

static std::shared_ptr<mock_connection>
add_connection(signalling::registration_handler &handler) {
  auto connection = std::make_shared<mock_connection>();
  handler.add(connection);
  signalling::registration registraion{"key"};
  connection->on_registration(registraion);
  return connection;
}

TEST_F(RegistrationHandler, Setup) {
  EXPECT_TRUE(handler.get_registered().empty());
}

TEST_F(RegistrationHandler, AddOffering) {
  add_connection(handler);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
}

TEST_F(RegistrationHandler, OfferingState) {
  const auto connection = add_connection(handler);
  EXPECT_TRUE(connection->state_offering_called);
}

TEST_F(RegistrationHandler, AddOfferAnswer) {
  auto first = add_connection(handler);
  auto second = add_connection(handler);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
  first->on_closed();
  EXPECT_TRUE(handler.get_registered().empty());
  EXPECT_TRUE(first.unique());
  EXPECT_TRUE(second.unique());
}

TEST_F(RegistrationHandler, OfferingClose) {
  const auto connection = add_connection(handler);
  connection->on_closed();
  EXPECT_TRUE(handler.get_registered().empty());
  EXPECT_TRUE(connection.unique());
}

