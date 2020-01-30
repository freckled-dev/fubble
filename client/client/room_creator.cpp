#include "room_creator.hpp"
#include "room.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room_creator::room_creator(boost::executor &executor) : executor(executor) {}

std::unique_ptr<room>
room_creator::create(std::unique_ptr<session::client> &&client_,
                     std::unique_ptr<session::room> &&room_) {
  (void)client_;
  (void)room_;
  // TODO create room_creator
  // return std::make_unique<room>(executor, std::move(client_),
  // std::move(room_));
  return nullptr;
}
