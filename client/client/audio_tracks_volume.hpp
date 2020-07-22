#ifndef UUID_8BB23974_D591_472A_A225_ABDB2CC6DB2A
#define UUID_8BB23974_D591_472A_A225_ABDB2CC6DB2A

#include "client/logger.hpp"

namespace client {
class rooms;
class audio_tracks_volume {
public:
  static std::unique_ptr<audio_tracks_volume> create(rooms &rooms_);

  audio_tracks_volume() = default;
  virtual ~audio_tracks_volume() = default;

  virtual void mute_all(bool muted) = 0;

protected:
};
} // namespace client

#endif
