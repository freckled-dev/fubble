#include "json_message.hpp"
#include <nlohmann/json.hpp>

using namespace signalling;

std::variant<offer, answer, ice_candidate>
json_message::parse(const std::string &message) const {
  std::variant<offer, answer, ice_candidate> result;
  return result;
}
std::string json_message::serialize(const offer &offer_) const { return ""; }
std::string json_message::serialize(const answer &answer_) const { return ""; }
std::string json_message::serialize(const ice_candidate &candidate) const {
  return "";
}
