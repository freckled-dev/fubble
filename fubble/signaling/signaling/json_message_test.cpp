#include "json_message.hpp"
#include <gtest/gtest.h>

struct JsonMessage : testing::Test {
  signaling::json_message parser;
};

template <class type_test>
void test_serialziation(signaling::json_message &parser, type_test &test) {
  std::string serialized = parser.serialize(test);
  auto parsed = parser.parse(serialized);
  EXPECT_EQ(boost::get<type_test>(parsed), test);
}

TEST_F(JsonMessage, Offer) {
  signaling::offer offer{"sdp offer"};
  test_serialziation(parser, offer);
}
TEST_F(JsonMessage, Answer) {
  signaling::answer answer{"sdp answer"};
  test_serialziation(parser, answer);
}
TEST_F(JsonMessage, IceCandidate) {
  signaling::ice_candidate ice_candidate{0, "mline", "sdp"};
  test_serialziation(parser, ice_candidate);
}
TEST_F(JsonMessage, Registration) {
  signaling::registration registration{"key", "token"};
  test_serialziation(parser, registration);
}
