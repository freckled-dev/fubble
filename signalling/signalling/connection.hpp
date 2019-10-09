#ifndef SIGNALLING_CONNECTION_HPP
#define SIGNALLING_CONNECTION_HPP

#include "answer.hpp"
#include "ice_candidate.hpp"
#include "offer.hpp"
#include "registration.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace signalling {
// TODO belongs to registration. make registraion namespace
class connection {
public:
  virtual ~connection() = default;

  // remove? instead make method remove. a pedon to add
  boost::signals2::signal<void()> on_closed;

  boost::signals2::signal<void(const registration &)> on_registration;
  boost::signals2::signal<void(const offer &)> on_offer;
  boost::signals2::signal<void(const answer &)> on_answer;
  boost::signals2::signal<void(const ice_candidate &)> on_ice_candidate;

  virtual void send_offer(const offer &send) = 0;
  virtual void send_ice_candidate(const ice_candidate &candidate) = 0;
  virtual void send_answer(const signalling::answer &answer_) = 0;
  virtual void send_state_offering() = 0;
  virtual void send_state_answering() = 0;
};
} // namespace signalling

#endif
