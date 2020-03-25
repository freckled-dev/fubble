#include "factory.hpp"
#include "room.hpp"
#include "users.hpp"

using namespace matrix;

std::unique_ptr<room> factory::create_room(client &client_,
                                           const std::string &room_id) {
  return std::make_unique<room>(client_, room_id);
}

std::unique_ptr<users> factory::create_users(client &client_) {

  return std::make_unique<users>(client_);
}
