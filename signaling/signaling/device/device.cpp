#include "device.hpp"
#include "signaling/connection.hpp"

using namespace signaling::device;

device::device(signaling::connection_ptr connection_, const std::string &token)
    : connection_(connection_), token{token} {
  callback_connections.emplace_back(connection_->on_offer.connect(
      [this](const auto &offer) { on_offer(offer); }));
  callback_connections.emplace_back(connection_->on_ice_candidate.connect(
      [this](const auto &candidate) { on_ice_candidate(candidate); }));
  callback_connections.emplace_back(connection_->on_want_to_negotiate.connect(
      [this](const auto &negotiation) { on_want_to_negotiate(negotiation); }));
  callback_connections.emplace_back(connection_->on_answer.connect(
      [this](const auto &answer) { on_answer(answer); }));
}

device::~device() = default;

void device::set_partner(const device_wptr &partner_) {
  BOOST_ASSERT(partner_.lock());
  partner = partner_;
  send_cache();
  negotiate();
}

void device::reset_partner() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  partner.reset();
  if (!active_negotiating)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "partner is getting reset, although `active_negotiating`!";
  active_negotiating = false;
  wants_to_negotiate = true;
}

void device::close() { connection_->close(); }

void device::send_offer(const signaling::offer &offer) {
  connection_->send_offer(offer);
}

void device::send_answer(const signaling::answer &answer) {
  connection_->send_answer(answer);
  BOOST_ASSERT(active_negotiating);
  active_negotiating = false;
  negotiate();
  if (active_negotiating)
    return;
  auto partner_strong = partner.lock();
  BOOST_ASSERT(partner_strong);
  if (!partner_strong)
    return;
  partner_strong->negotiate();
}

void device::send_ice_candidate(const signaling::ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

bool device::get_wants_to_negotiate() { return wants_to_negotiate; }

bool device::get_actice_negotiating() { return active_negotiating; }

void device::negotiate() {
  if (!wants_to_negotiate)
    return;
  if (active_negotiating)
    return;
  auto partner_strong = partner.lock();
  if (!partner_strong)
    return;
  if (partner_strong->get_actice_negotiating())
    return;
  active_negotiating = true;
  wants_to_negotiate = false;
  connection_->send_do_offer();
}

std::string device::get_token() const { return token; }

void device::on_want_to_negotiate(const signaling::want_to_negotiate &) {
  wants_to_negotiate = true;
  auto partner_strong = partner.lock();
  if (!partner_strong)
    return;
  negotiate();
}

void device::on_answer(const signaling::answer &answer_) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_answer(answer_);
    return;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << __FUNCTION__ << "device got no partner!";
  BOOST_ASSERT(!answer_cache);
  answer_cache = answer_;
}

void device::on_offer(const signaling::offer &offer_) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_offer(offer_);
    return;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << __FUNCTION__ << "device got no partner!";
  BOOST_ASSERT(!offer_cache);
  offer_cache = offer_;
}

void device::on_ice_candidate(const signaling::ice_candidate &candidate) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_ice_candidate(candidate);
    return;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning) << "device got no partner!";
  candidates_cache.push_back(candidate);
}

void device::send_cache() {
  auto partner_strong = partner.lock();
  BOOST_ASSERT(partner_strong);
  for (const auto &candidate : candidates_cache)
    partner_strong->send_ice_candidate(candidate);
  candidates_cache.clear();
  if (offer_cache)
    partner_strong->send_offer(offer_cache.value());
  offer_cache.reset();
  if (answer_cache)
    partner_strong->send_answer(answer_cache.value());
  answer_cache.reset();
}
