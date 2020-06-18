#include "own_participant.hpp"
#include "own_media.hpp"

using namespace client;

own_participant::own_participant(matrix::user &matrix_participant,
                                 own_media &own_media_)
    : participant(matrix_participant), own_media_(own_media_) {}

own_participant::~own_participant() = default;

boost::future<void> own_participant::close() {
  return boost::make_ready_future();
}

own_participant::videos_type own_participant::get_videos() const {
  return own_media_.get_videos();
}

own_participant::audios_type own_participant::get_audios() const {
  audios_type result;
  auto own_audio = own_media_.get_audio();
  if (own_audio)
    result.push_back(own_audio);
  return result;
}
