#ifndef SIGNALLING_CONNECTION_HPP
#define SIGNALLING_CONNECTION_HPP

#include "ice_candidate.hpp"
#include "offer.hpp"
#include "registration.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace signalling {
class connection {
public:
  virtual ~connection() = default;

  boost::signals2::signal<void()> on_closed;

  std::function<void(const registration &)> on_registration;
  std::function<void(const offer &)> on_offer;
  std::function<void(const ice_candidate &)> on_ice_candidate;

  virtual void send_offer(const offer &send) = 0;
  virtual void send_ice_candidate(const ice_candidate &candidate) = 0;
  virtual void send_answer() = 0;
  virtual void send_state_offering() = 0;
  virtual void send_state_answering() = 0;
};
} // namespace signalling

#endif
