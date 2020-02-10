#include "own_participant.hpp"
#include "own_media.hpp"

using namespace client;

own_participant::own_participant(
    const session::participant &session_participant, own_media &own_media_)
    : session_participant(session_participant), own_media_(own_media_) {}

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

own_participant::videos_type own_participant::get_videos() const {
  return own_media_.get_videos();
}
