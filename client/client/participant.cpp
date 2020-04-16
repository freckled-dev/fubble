#include "participant.hpp"

using namespace client;

participant::participant(session::participant &session_participant)
    : session_participant(session_participant) {
  connection_update =
      session_participant.on_update.connect([this] { update(); });
}

participant::~participant() = default;

std::string participant::get_id() const { return id; }

std::string participant::get_name() const {
  return session_participant.get_name();
}

void participant::update() { on_name_changed(session_participant.get_name()); }
