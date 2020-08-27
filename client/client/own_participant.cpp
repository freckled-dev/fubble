#include "own_participant.hpp"
#include "own_media.hpp"

using namespace client;

own_participant::own_participant(matrix::user &matrix_participant,
                                 own_media &own_media_)
    : participant(matrix_participant), own_media_(own_media_) {
  own_media_.get_videos().on_added.connect(
      [this](auto &source) { on_video_added(source); });
  own_media_.get_videos().on_removed.connect(
      [this](auto &source) { on_video_removed(source); });
  // TODO add and removal of audio!
}

own_participant::~own_participant() = default;

boost::future<void> own_participant::close() {
  return boost::make_ready_future();
}

own_participant::videos_type own_participant::get_videos() const {
  return own_media_.get_videos().get_all();
}

own_participant::audios_type own_participant::get_audios() const {
  audios_type result;
  auto own_audio = own_media_.get_audio();
  if (own_audio)
    result.push_back(own_audio);
  return result;
}
