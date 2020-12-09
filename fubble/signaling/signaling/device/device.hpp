#ifndef SIGNALING_DEVICE_DEVICE_HPP
#define SIGNALING_DEVICE_DEVICE_HPP

#include "signaling/answer.hpp"
#include "signaling/connection_ptr.hpp"
#include "signaling/ice_candidate.hpp"
#include "signaling/logger.hpp"
#include "signaling/offer.hpp"
#include "signaling/want_to_negotiate.hpp"
#include <boost/signals2/connection.hpp>
#include <functional>
#include <memory>
#include <boost/optional.hpp>
#include <vector>

namespace signaling::device {
class device;
using device_wptr = std::weak_ptr<device>;
class device {
public:
  device(connection_ptr connection_, const std::string &token);
  virtual ~device();

  void set_partner(const device_wptr &partner);
  void reset_partner();
  void close();
  void send_offer(const offer &offer);
  void send_answer(const answer &answer_);
  void send_ice_candidate(const ice_candidate &candidate);
  bool get_wants_to_negotiate();
  bool get_actice_negotiating();
  void negotiate();
  std::string get_token() const;

protected:
  virtual void on_want_to_negotiate(const want_to_negotiate &negotiation);
  virtual void on_answer(const answer &answer_);
  virtual void on_offer(const offer &offer_);
  virtual void on_ice_candidate(const ice_candidate &candidate);
  void send_cache();

  signaling::logger logger{"device"};
  connection_ptr connection_;
  // TODO remove weakptr to partner. partner shall be a ref. ownership shall
  // extrusive. or make it a shared_ptr
  device_wptr partner;
  std::vector<boost::signals2::scoped_connection> callback_connections;
  bool wants_to_negotiate{};
  bool active_negotiating{};
  std::string token;

  boost::optional<offer> offer_cache;
  boost::optional<answer> answer_cache;
  std::vector<ice_candidate> candidates_cache;
};
} // namespace signaling::device

#endif
