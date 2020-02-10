#include "own_participant.hpp"

using namespace client;

own_participant::own_participant(
    const session::participant &session_participant)
    : session_participant(session_participant) {}

own_participant::~own_participant() = default;

std::string own_participant::get_id() const { return session_participant.id; }

std::string own_participant::get_name() const {
  return session_participant.name;
}

void own_participant::update(const session::participant &update) {
  BOOST_ASSERT(update.id == session_participant.id);
  if (get_name() == update.name)
    return;
  session_participant = update;
  on_name_changed(session_participant.name);
}
