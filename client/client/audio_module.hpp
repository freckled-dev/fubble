#ifndef UUID_2761C87F_C48F_4545_BAED_5C5E847EDF9C
#define UUID_2761C87F_C48F_4545_BAED_5C5E847EDF9C

#include "rtc/google/module.hpp"

namespace client {
class own_audio_track;
class add_audio_to_connection;
class audio_tracks_volume;
class audio_module {
public:
  struct config {
    // TODO
  };

  audio_module(std::shared_ptr<utils::executor_module> executor_module,
               std::shared_ptr<rtc::google::module> rtc_module,
               const config &config_);

protected:
  std::shared_ptr<own_audio_track> own_audio_track_;
  std::shared_ptr<add_audio_to_connection> audio_track_adder;
  std::shared_ptr<audio_tracks_volume> audio_tracks_volume;
};
} // namespace client

#endif
