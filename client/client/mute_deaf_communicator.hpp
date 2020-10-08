#ifndef UUID_3DE1BF68_04C1_4691_BB73_D5E35CB3A195
#define UUID_3DE1BF68_04C1_4691_BB73_D5E35CB3A195

#include "client/audio_tracks_volume.hpp"
#include "rooms.hpp"
#include <memory>

namespace client {
class mute_deaf_communicator {
public:
  virtual ~mute_deaf_communicator() = default;

  static std::unique_ptr<mute_deaf_communicator>
  create(std::shared_ptr<rooms> rooms,
         std::shared_ptr<audio_volume> audio_volume_);
};
} // namespace client

#endif
