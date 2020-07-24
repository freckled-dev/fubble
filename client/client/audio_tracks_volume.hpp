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

  virtual void mute_all_except_self(bool muted) = 0;
  virtual bool get_all_muted_except_self() const = 0;

  virtual void mute_self(bool muted) = 0;
  virtual bool get_self_muted() = 0;

  virtual void deafen(bool deafed) = 0;
  virtual bool get_deafen() = 0;

#if 0 // TODO
  virtual void set_volume(std::string id, double volume);
  virtual void mute(std::string id, bool muted);
#endif

protected:
};
} // namespace client

#endif
