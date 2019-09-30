#include "creator.hpp"
#include "answering.hpp"
#include "offering.hpp"

using namespace signalling::device;

creator::creator(boost::generic_executor_ref &executor) : executor(executor) {}

offering_ptr creator::create_offering(connection_ptr connection_) {
  return std::make_unique<offering>(executor, connection_);
}
answering_ptr creator::create_answering(connection_ptr connection_) {
  return std::make_unique<answering>(executor, connection_);
}
