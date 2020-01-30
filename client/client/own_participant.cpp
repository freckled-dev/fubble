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
