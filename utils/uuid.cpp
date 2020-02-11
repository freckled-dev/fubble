#include "uuid.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string uuid::generate() {
  auto label = boost::uuids::random_generator()();
  return boost::uuids::to_string(label);
}

