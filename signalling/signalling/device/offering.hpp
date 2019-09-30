#ifndef SIGNALLING_DEVICE_OFFERING_HPP
#define SIGNALLING_DEVICE_OFFERING_HPP

#include "answering_ptr.hpp"
#include "signalling/connection_ptr.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/offer.hpp"
#include <boost/thread/executors/generic_executor_ref.hpp>
#include <optional>

namespace signalling::device {
class offering {
public:
  offering(boost::generic_executor_ref &executor, connection_ptr connection_);
  virtual ~offering();

  void set_partner(const answering_ptr &partner);

protected:
  virtual void on_offer(const offer &offer_);
  virtual void on_ice_candidate(const ice_candidate &candidate);

  boost::generic_executor_ref executor;
  connection_ptr connection_;

  offer offer_;
  std::vector<ice_candidate> candidates;
};
} // namespace signalling::device

#endif
