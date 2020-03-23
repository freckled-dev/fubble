#include "room_factory.hpp"
#include "room.hpp"

using namespace matrix;

std::unique_ptr<room> room_factory::create(client &client_,
                                           const std::string &room_id) {
  return std::make_unique<room>(client_, room_id);
}

