#ifndef SIGNALLING_DEVICE_DEVICE_HPP
#define SIGNALLING_DEVICE_DEVICE_HPP

#include "logging/logger.hpp"
#include "signalling/answer.hpp"
#include "signalling/connection_ptr.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/offer.hpp"
#include "signalling/want_to_negotiate.hpp"
#include <boost/signals2/connection.hpp>
#include <functional>
#include <memory>

namespace signalling::device {
class device;
using device_wptr = std::weak_ptr<device>;
class device {
public:
  device(connection_ptr connection_);
  virtual ~device();

  void set_partner(const device_wptr &partner);
  void close();
  void send_offer(const offer &offer);
  void send_answer(const answer &answer_);
  void send_ice_candidate(const ice_candidate &candidate);

protected:
  virtual void on_want_to_negotiate(const want_to_negotiate &answer_);
  virtual void on_answer(const answer &answer_);
  virtual void on_offer(const offer &offer_);
  virtual void on_ice_candidate(const ice_candidate &candidate);

  logging::logger logger;
  connection_ptr connection_;
  // TODO remove weakptr to partner. partner shall be a ref. ownership shall
  // extrusive
  device_wptr partner;
  std::vector<boost::signals2::scoped_connection> callback_connections;
};
} // namespace signalling::device

#endif
