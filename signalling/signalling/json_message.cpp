#include "json_message.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using namespace signalling;

json_message::json_message() = default;

json_message::~json_message() = default;

json_message::invalid_type::invalid_type(const std::string &type)
    : std::runtime_error(
          fmt::format("invalid json message type. type:'{}'", type)) {}

json_message::messages_type
json_message::parse(const std::string &message) const {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << fmt::format("parsing the message: '{}'", message);
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
    result.mlineindex = json["mlineindex"];
    result.mid = json["mid"];
    return result;
  }
  if (type == "create_offer") {
    create_offer result;
    return result;
  }
  if (type == "registration") {
    registration result;
    result.key = json["key"];
    return result;
  }
  if (type == "want_to_negotiate") {
    want_to_negotiate result;
    return result;
  }
  throw invalid_type(type);
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
  nlohmann::json result = {{"type", "ice_candidate"},
                           {"sdp", candidate.sdp},
                           {"mlineindex", candidate.mlineindex},
                           {"mid", candidate.mid}};
  return result.dump();
}
std::string json_message::serialize(const create_offer &) const {
  nlohmann::json result = {{"type", "create_offer"}};
  return result.dump();
}
std::string json_message::serialize(const registration &registration_) const {
  nlohmann::json result = {{"type", "registration"},
                           {"key", registration_.key}};
  return result.dump();
}

std::string json_message::serialize(const want_to_negotiate &) const {
  nlohmann::json result = {{"type", "want_to_negotiate"}};
  return result.dump();
}
