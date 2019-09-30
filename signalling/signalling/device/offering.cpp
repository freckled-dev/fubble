#include "offering.hpp"
#include "signalling/connection.hpp"

using namespace signalling::device;

offering::offering(boost::generic_executor_ref &executor,
                   connection_ptr connection_)
    : executor(executor), connection_(connection_) {
  connection_->send_state_offering();
  connection_->on_offer = [this](const auto &offer) { on_offer(offer_); };
  connection_->on_ice_candidate = [this](const auto &candidate) {
    on_ice_candidate(candidate);
  };
}

offering::~offering() = default;

void offering::set_partner(const answering_ptr &partner) {}

void offering::on_offer(const offer &offer_) {}
void offering::on_ice_candidate(const ice_candidate &candidate) {}
