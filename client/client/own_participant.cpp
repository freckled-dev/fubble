#include "own_participant.hpp"
#include "own_media.hpp"

using namespace client;

own_participant::own_participant(session::participant &session_participant,
                                 own_media &own_media_)
    : participant(session_participant), own_media_(own_media_) {}

own_participant::~own_participant() = default;

own_participant::videos_type own_participant::get_videos() const {
  return own_media_.get_videos();
}
