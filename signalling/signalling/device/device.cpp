#include "device.hpp"
#include "signalling/connection.hpp"

using namespace signalling::device;

device::device(connection_ptr connection_) : connection_(connection_) {
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
  auto strong_partner = partner_.lock();
  BOOST_ASSERT(strong_partner);
  partner = partner_;
}

void device::send_answer(const answer &offer) {
  connection_->send_answer(offer);
}

void device::close() { connection_->close(); }

void device::send_ice_candidate(const ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

void device::on_offer(const offer &work) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_offer(work);
    return;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning) << "device got no partner!";
  BOOST_ASSERT(false);
}

void device::on_ice_candidate(const ice_candidate &candidate) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_ice_candidate(candidate);
    return;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning) << "device got no partner!";
  BOOST_ASSERT(false);
}
