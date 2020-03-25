#include "factory.hpp"
#include "room.hpp"

using namespace matrix;

std::unique_ptr<room> factory::create_room(client &client_,
                                           const std::string &room_id) {
  return std::make_unique<room>(client_, room_id);
}

