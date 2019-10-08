#ifndef SIGNALLING_DEVICE_OFFERING_HPP
#define SIGNALLING_DEVICE_OFFERING_HPP

#include "answering_ptr.hpp"
#include "signalling/answer.hpp"
#include "signalling/connection_ptr.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/offer.hpp"
#include <boost/signals2/connection.hpp>
#include <boost/thread/executors/executor.hpp>
#include <optional>

namespace signalling::device {
class offering {
public:
  offering(connection_ptr connection_);
  virtual ~offering();

  void set_partner(const answering_wptr &partner);

  void send_answer(const answer &answer_);
  void send_ice_candidate(const ice_candidate &candidate);

protected:
  virtual void on_offer(const offer &offer_);
  virtual void on_ice_candidate(const ice_candidate &candidate);

  connection_ptr connection_;
  answering_wptr partner;

  std::optional<offer> offer_;
  std::vector<ice_candidate> candidates;

  std::vector<boost::signals2::scoped_connection> callback_connections;
};
} // namespace signalling::device

#endif
