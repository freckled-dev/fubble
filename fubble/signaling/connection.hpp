#ifndef SIGNALING_CONNECTION_HPP
#define SIGNALING_CONNECTION_HPP

#include "answer.hpp"
#include "ice_candidate.hpp"
#include "offer.hpp"
#include "registration.hpp"
#include "want_to_negotiate.hpp"
#include <sigslot/signal.hpp>
#include <boost/thread/future.hpp>

namespace signaling {
// this connection is only getting used on the server side
class connection {
public:
  virtual ~connection() = default;

  // remove? instead make method remove. a pedon to add
  sigslot::signal<> on_closed;

  sigslot::signal<const registration &> on_registration;
  sigslot::signal<const offer &> on_offer;
  sigslot::signal<const answer &> on_answer;
  sigslot::signal<const ice_candidate &> on_ice_candidate;
  sigslot::signal<const want_to_negotiate &> on_want_to_negotiate;

  virtual void close() = 0;
  // TODO all send methods shall return a future. #263. handle the fails
  virtual void send_offer(const offer &send) = 0;
  virtual void send_ice_candidate(const ice_candidate &candidate) = 0;
  virtual void send_answer(const signaling::answer &answer_) = 0;
  virtual void send_do_offer() = 0;
};
} // namespace signaling

#endif
