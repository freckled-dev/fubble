#ifndef SIGNALING_JSON_MESSAGE_HPP
#define SIGNALING_JSON_MESSAGE_HPP

#include "answer.hpp"
#include "create_offer.hpp"
#include "ice_candidate.hpp"
#include "logger.hpp"
#include "offer.hpp"
#include "registration.hpp"
#include "want_to_negotiate.hpp"
#include <boost/variant.hpp>
#include <stdexcept>

namespace signaling {
class json_message {
public:
  json_message();
  ~json_message();

  struct invalid_type : std::runtime_error {
    invalid_type(const std::string &type);
  };

  using messages_type =
      boost::variant<offer, answer, ice_candidate, create_offer, registration,
                     want_to_negotiate>;
  messages_type parse(const std::string &message) const;

  std::string serialize(const offer &offer_) const;
  std::string serialize(const answer &answer_) const;
  std::string serialize(const ice_candidate &candidate) const;
  std::string serialize(const create_offer &offering) const;
  std::string serialize(const registration &registration_) const;
  std::string serialize(const want_to_negotiate &negotiate) const;

private:
  mutable class logger logger { "json_message" };
};
} // namespace signaling

#endif
