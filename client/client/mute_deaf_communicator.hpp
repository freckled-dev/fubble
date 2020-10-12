#ifndef UUID_3DE1BF68_04C1_4691_BB73_D5E35CB3A195
#define UUID_3DE1BF68_04C1_4691_BB73_D5E35CB3A195

#include "client/audio_tracks_volume.hpp"
#include "rooms.hpp"
#include <memory>

namespace client {
class mute_deaf_communicator {
public:
  virtual ~mute_deaf_communicator() = default;

  virtual bool is_deafed(const std::string &id) const = 0;
  virtual bool is_muted(const std::string &id) const = 0;
  boost::signals2::signal<void(std::string id, bool deafed)> on_deafed;
  boost::signals2::signal<void(std::string id, bool muted)> on_muted;

  static std::unique_ptr<mute_deaf_communicator>
  create(std::shared_ptr<rooms> rooms,
         std::shared_ptr<audio_volume> audio_volume_);
};
} // namespace client

#endif
