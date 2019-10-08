#include "creator.hpp"
#include "answering.hpp"
#include "offering.hpp"

using namespace signalling::device;

creator::creator(boost::executor &executor) : executor(executor) {}

offering_ptr creator::create_offering(connection_ptr connection_) {
  return std::make_unique<offering>(connection_);
}
answering_ptr creator::create_answering(connection_ptr connection_,
                                        offering_ptr partner) {
  return std::make_unique<answering>(connection_, partner);
}
