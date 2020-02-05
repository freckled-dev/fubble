#ifndef SIGNALLING_CONNECTION_HPP
#define SIGNALLING_CONNECTION_HPP

#include "answer.hpp"
#include "ice_candidate.hpp"
#include "offer.hpp"
#include "registration.hpp"
#include "want_to_negotiate.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace signalling {
class connection {
public:
  virtual ~connection() = default;

  // remove? instead make method remove. a pedon to add
  boost::signals2::signal<void()> on_closed;

  boost::signals2::signal<void(const registration &)> on_registration;
  boost::signals2::signal<void(const offer &)> on_offer;
  boost::signals2::signal<void(const answer &)> on_answer;
  boost::signals2::signal<void(const ice_candidate &)> on_ice_candidate;
  boost::signals2::signal<void(const want_to_negotiate &)> on_want_to_negotiate;

  virtual void close() = 0;
  virtual void send_offer(const offer &send) = 0;
  virtual void send_ice_candidate(const ice_candidate &candidate) = 0;
  virtual void send_answer(const signalling::answer &answer_) = 0;
  virtual void send_do_offer() = 0;
  virtual void send_do_answer() = 0;
};
} // namespace signalling

#endif
