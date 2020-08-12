#include "registration_handler.hpp"
#include "signalling/connection.hpp"
#include "signalling/device/creator.hpp"
#include "utils/uuid.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

struct RegistrationHandler : testing::Test {
  boost::executor_adaptor<boost::inline_executor> executor;
  signalling::device::creator creator{executor};
  signalling::registration_handler handler{creator};
};

struct mock_connection final : signalling::connection {
  ~mock_connection() final { on_closed(); }
  std::optional<signalling::offer> offer;
  void send_offer(const signalling::offer &send) final {
    EXPECT_FALSE(offer);
    offer = send;
  }
  std::vector<signalling::ice_candidate> candidates;
  void send_ice_candidate(const signalling::ice_candidate &candidate) final {
    candidates.push_back(candidate);
  }
  std::optional<signalling::answer> answer;
  void send_answer(const signalling::answer &answer_) final {
    answer = answer_;
  }
  bool state_offering_called{};
  void send_do_offer() final { state_offering_called = true; }
  bool close_called{};
  void close() final {
    EXPECT_FALSE(close_called);
    close_called = true;
  }
};

namespace {
std::shared_ptr<mock_connection>
add_connection(signalling::registration_handler &handler,
               std::string token = uuid::generate()) {
  auto connection = std::make_shared<mock_connection>();
  handler.add(connection);
  signalling::registration registration{"key", token};
  connection->on_registration(registration);
  return connection;
}

std::shared_ptr<mock_connection>
add_offering_connection(signalling::registration_handler &handler) {
  auto connection = add_connection(handler);
  connection->on_want_to_negotiate({});
  return connection;
}
} // namespace

TEST_F(RegistrationHandler, Setup) {
  EXPECT_TRUE(handler.get_registered().empty());
}

TEST_F(RegistrationHandler, AddOffering) {
  auto first = add_connection(handler);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
}

TEST_F(RegistrationHandler, OfferingState) {
  const auto connection = add_offering_connection(handler);
  add_connection(handler);
  EXPECT_TRUE(connection->state_offering_called);
}

TEST_F(RegistrationHandler, AddOfferAnswer) {
  auto first = add_connection(handler);
  auto second = add_connection(handler);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
}

TEST_F(RegistrationHandler, OfferAnswerOneCloses) {
  auto first = add_connection(handler);
  auto second = add_connection(handler);
  first->on_closed();
  EXPECT_FALSE(handler.get_registered().empty());
  EXPECT_TRUE(first.unique());
  EXPECT_FALSE(second.unique());
}

TEST_F(RegistrationHandler, OfferAnswerBothClose) {
  auto first = add_connection(handler);
  auto second = add_connection(handler);
  first->on_closed();
  second->on_closed();
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

TEST_F(RegistrationHandler, OfferingStateSend) {
  const auto connection = add_offering_connection(handler);
  add_connection(handler);
  EXPECT_TRUE(connection->state_offering_called);
}

TEST_F(RegistrationHandler, OnlyOneDoOfferingSend) {
  auto first = add_offering_connection(handler);
  const auto connection = add_connection(handler);
  EXPECT_TRUE(first->state_offering_called);
  EXPECT_FALSE(connection->state_offering_called);
}

TEST_F(RegistrationHandler, OfferSend) {
  const auto offering = add_offering_connection(handler);
  const auto answering = add_connection(handler);
  const signalling::offer offer{"sdp"};
  offering->on_offer(offer);
  EXPECT_EQ(offer, answering->offer.value());
  EXPECT_FALSE(offering->offer);
}

TEST_F(RegistrationHandler, OfferLateSend) {
  const auto offering = add_offering_connection(handler);
  const auto answering = add_connection(handler);
  const signalling::offer offer{"sdp"};
  offering->on_offer(offer);
  EXPECT_EQ(offer, answering->offer.value());
  EXPECT_FALSE(offering->offer);
}

TEST_F(RegistrationHandler, AnswerSend) {
  const auto offering = add_offering_connection(handler);
  const auto answering = add_connection(handler);
  const signalling::answer answer{"sdp"};
  answering->on_answer(answer);
  EXPECT_EQ(answer, offering->answer.value());
  EXPECT_FALSE(answering->answer);
}

TEST_F(RegistrationHandler, IceCandidateToAnswer) {
  const auto offering = add_connection(handler);
  const auto answering = add_connection(handler);
  signalling::ice_candidate candidate{0, "mline", "candidate"};
  offering->on_ice_candidate(candidate);
  EXPECT_EQ(answering->candidates.front(), candidate);
}

TEST_F(RegistrationHandler, IceCandidateToOffer) {
  const auto offering = add_connection(handler);
  const auto answering = add_connection(handler);
  signalling::ice_candidate candidate{0, "mline", "candidate"};
  answering->on_ice_candidate(candidate);
  EXPECT_EQ(offering->candidates.front(), candidate);
}

TEST_F(RegistrationHandler, IceCandidatesToAnswer) {
  const auto offering = add_connection(handler);
  const auto answering = add_connection(handler);
  signalling::ice_candidate candidate{0, "mline", "candidate"};
  offering->on_ice_candidate(candidate);
  offering->on_ice_candidate(candidate);
  EXPECT_EQ(answering->candidates.size(), std::size_t{2});
}

TEST_F(RegistrationHandler, Close) {
  const auto offering = add_connection(handler);
  const auto answering = add_connection(handler);
  offering->on_closed();
  EXPECT_FALSE(answering->close_called);
}

TEST_F(RegistrationHandler, Reconnect) {
  auto offering_token = uuid::generate();
  const auto offering = add_connection(handler, offering_token);
  const auto answering = add_connection(handler);
  const auto replacing = add_connection(handler, offering_token);
  EXPECT_TRUE(offering->close_called);
  EXPECT_FALSE(answering->close_called);
  EXPECT_FALSE(replacing->close_called);
}

TEST_F(RegistrationHandler, Reconnect2) {
  auto offering_token = uuid::generate();
  const auto offering = add_connection(handler, offering_token);
  const auto answering = add_connection(handler);
  offering->on_closed();
  const auto replacing = add_connection(handler, offering_token);
  EXPECT_FALSE(answering->close_called);
  EXPECT_FALSE(replacing->close_called);
}

TEST_F(RegistrationHandler, ReconnectResend) {
  auto offering_token = uuid::generate();
  const auto offering = add_connection(handler, offering_token);
  const auto answering = add_connection(handler);
  offering->on_closed();
  answering->on_ice_candidate(
      signalling::ice_candidate{0, "mline", "candidate"});
  const auto replacing = add_connection(handler, offering_token);
  EXPECT_FALSE(replacing->candidates.empty());
}
