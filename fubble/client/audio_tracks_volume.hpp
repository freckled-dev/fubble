#ifndef UUID_8BB23974_D591_472A_A225_ABDB2CC6DB2A
#define UUID_8BB23974_D591_472A_A225_ABDB2CC6DB2A

#include "fubble/client/logger.hpp"
#include <fubble/utils/signal.hpp>

namespace client {
class add_audio_to_connection;
class rooms;
class tracks_adder;
class own_audio_track;
class audio_volume {
public:
  audio_volume() = default;
  virtual ~audio_volume() = default;

  virtual void mute_all_except_self(bool muted) = 0;
  virtual bool get_all_muted_except_self() const = 0;

  virtual void mute_self(bool muted) = 0;
  virtual bool get_self_muted() = 0;

  virtual void deafen(bool deafed) = 0;
  virtual bool get_deafen() = 0;

  virtual void set_volume(std::string id, double volume) = 0;
  virtual double get_volume(std::string id) const = 0;

  virtual void mute(std::string id, bool muted) = 0;
  virtual bool get_muted(std::string id) const = 0;

  utils::signal::signal<bool> on_muted;
  utils::signal::signal<bool> on_deafed;
};
class audio_tracks_volume : public audio_volume {
public:
  static std::unique_ptr<audio_tracks_volume>
  create(rooms &rooms_, tracks_adder &tracks_adder_,
         const std::shared_ptr<add_audio_to_connection> &audio_track_adder,
         own_audio_track &own_audio_track_);
};
} // namespace client

#endif
