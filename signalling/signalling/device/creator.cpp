#include "creator.hpp"
#include "answering.hpp"
#include "offering.hpp"

using namespace signalling::device;

offering_ptr creator::create_offering(connection_ptr connection_) {
  return std::make_unique<offering>(connection_);
}
answering_ptr creator::create_answering(connection_ptr connection_) {
  return std::make_unique<answering>(connection_);
}
