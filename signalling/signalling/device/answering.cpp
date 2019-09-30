#include "answering.hpp"
#include "offering.hpp"
#include "signalling/connection.hpp"

using namespace signalling::device;

answering::answering(connection_ptr connection_, offering_ptr offering)
    : connection_(connection_), partner(offering) {}

answering::~answering() = default;

void answering::partner_closed() { partner.reset(); }

void answering::send_offer(const offer &offer) {
  connection_->send_offer(offer);
}
void answering::send_ice_candidate(const ice_candidate &candidate) {
  connection_->send_ice_candidate(candidate);
}

void answering::on_answer(const answer &answer_) {}
void answering::on_ice_candidate(const ice_candidate &candidate) {}
