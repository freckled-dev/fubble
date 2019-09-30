#ifndef SIGNALLING_DEVICE_ANSWERING_HPP
#define SIGNALLING_DEVICE_ANSWERING_HPP

#include "offering_ptr.hpp"
#include "signalling/answer.hpp"
#include "signalling/connection_ptr.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/offer.hpp"
#include <boost/thread/executors/generic_executor_ref.hpp>

namespace signalling::device {
class answering {
public:
  answering(connection_ptr connection_, offering_ptr partner);
  virtual ~answering();

  void partner_closed();
  void send_offer(const offer &offer);
  void send_ice_candidate(const ice_candidate &candidate);

protected:
  virtual void on_answer(const answer &answer_);
  virtual void on_ice_candidate(const ice_candidate &candidate);

  connection_ptr connection_;
  offering_wptr partner;
};
} // namespace signalling::device

#endif
