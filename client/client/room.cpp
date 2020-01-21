#include "room.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room::room(boost::executor &session_thread,
           std::unique_ptr<session::client> client_,
           std::unique_ptr<session::room> room_)
    : session_thread(session_thread), client_(std::move(client_)),
      room_(std::move(room_)) {}

room::~room() = default;

session::room &room::get_session_room() { return *room_; }

boost::executor &room::get_session_thread() { return session_thread; }
