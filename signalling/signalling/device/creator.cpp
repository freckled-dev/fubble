#include "creator.hpp"
#include "answering.hpp"
#include "offering.hpp"

using namespace signalling::device;

std::unique_ptr<offering> creator::create_offering(connection_ptr connection_) {
  return std::make_unique<offering>(connection_);
}
std::unique_ptr<answering>
creator::create_answering(connection_ptr connection_) {
  return std::make_unique<answering>(connection_);
}
