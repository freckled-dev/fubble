#include "offering.hpp"
#include "answering.hpp"
#include "signalling/connection.hpp"

using namespace signalling::device;

offering::offering(connection_ptr connection_) : connection_(connection_) {
  connection_->send_do_offer();
  callback_connections.emplace_back(connection_->on_offer.connect(
      [this](const auto &offer) { on_offer(offer); }));
  callback_connections.emplace_back(connection_->on_ice_candidate.connect(
      [this](const auto &candidate) { on_ice_candidate(candidate); }));
}

offering::~offering() = default;

void offering::set_partner(const answering_wptr &partner_) {
  auto strong_partner = partner_.lock();
  BOOST_ASSERT(strong_partner);
  partner = partner_;
  if (offer_)
    strong_partner->send_offer(offer_.value());
  for (const auto &candidate : candidates)
    strong_partner->send_ice_candidate(candidate);
}

void offering::send_answer(const answer &offer) {
  connection_->send_answer(offer);
}

void offering::close() { connection_->close(); }

void offering::send_ice_candidate(const ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

void offering::on_offer(const offer &work) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_offer(work);
    return;
  }
  offer_ = work;
}

void offering::on_ice_candidate(const ice_candidate &candidate) {
  auto partner_strong = partner.lock();
  if (partner_strong) {
    partner_strong->send_ice_candidate(candidate);
    return;
  }
  candidates.push_back(candidate);
}
