#include "json_message.hpp"
#include <nlohmann/json.hpp>

using namespace signalling;

std::variant<offer, answer, ice_candidate>
json_message::parse(const std::string &message) const {
  std::variant<offer, answer, ice_candidate> result;
  auto json = nlohmann::json::parse(message);
  auto type = json["type"];
  if (type == "offer") {
    offer result;
    result.sdp = json["sdp"];
    return result;
  }
  if (type == "answer") {
    answer result;
    result.sdp = json["sdp"];
    return result;
  }
  if (type == "ice_candidate") {
    ice_candidate result;
    result.sdp = json["sdp"];
    return result;
  }
  return result;
}
std::string json_message::serialize(const offer &offer_) const {
  nlohmann::json result = {{"type", "offer"}, {"sdp", offer_.sdp}};
  return result.dump();
}
std::string json_message::serialize(const answer &answer_) const {
  nlohmann::json result = {{"type", "answer"}, {"sdp", answer_.sdp}};
  return result.dump();
}
std::string json_message::serialize(const ice_candidate &candidate) const {
  nlohmann::json result = {{"type", "ice_candidate"}, {"sdp", candidate.sdp}};
  return result.dump();
}
