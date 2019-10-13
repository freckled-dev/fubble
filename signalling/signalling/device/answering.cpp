#include "answering.hpp"
#include "offering.hpp"
#include "signalling/connection.hpp"

using namespace signalling::device;

answering::answering(connection_ptr connection_, offering_ptr offering)
    : connection_(connection_), partner(offering) {
  connection_->send_state_answering();
  callback_connections.emplace_back(connection_->on_answer.connect(
      [this](const auto &answer) { on_answer(answer); }));
  callback_connections.emplace_back(connection_->on_ice_candidate.connect(
      [this](const auto &candidate) { on_ice_candidate(candidate); }));
}

answering::~answering() = default;

void answering::close() { connection_->close(); }

void answering::send_offer(const offer &offer) {
  connection_->send_offer(offer);
}
void answering::send_ice_candidate(const ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

void answering::on_answer(const answer &answer_) {
  const auto strong_partner = partner.lock();
  BOOST_ASSERT(strong_partner);
  strong_partner->send_answer(answer_);
}
void answering::on_ice_candidate(const ice_candidate &candidate) {
  const auto strong_partner = partner.lock();
  BOOST_ASSERT(strong_partner);
  strong_partner->send_ice_candidate(candidate);
}
