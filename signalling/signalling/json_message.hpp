#ifndef SIGNALLING_JSON_MESSAGE_HPP
#define SIGNALLING_JSON_MESSAGE_HPP

#include "answer.hpp"
#include "ice_candidate.hpp"
#include "offer.hpp"
#include <variant>

namespace signalling {
class json_message {
public:
  std::variant<offer, answer, ice_candidate>
  parse(const std::string &message) const;

  std::string serialize(const offer &offer_) const;
  std::string serialize(const answer &answer_) const;
  std::string serialize(const ice_candidate &candidate) const;
};
} // namespace signalling

#endif
