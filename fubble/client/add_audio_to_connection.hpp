#ifndef UUID_FD8A9B65_2453_4AB1_A862_41616BC96172
#define UUID_FD8A9B65_2453_4AB1_A862_41616BC96172

#include <fubble/client/logger.hpp>
#include <fubble/client/track_adder.hpp>
#include <fubble/rtc/audio_track.hpp>

namespace client {
class add_audio_to_connection : public track_adder {
public:
  virtual std::shared_ptr<rtc::audio_track> get_track() = 0;

  static std::unique_ptr<add_audio_to_connection>
  create(std::shared_ptr<rtc::audio_track> source);
  static std::unique_ptr<add_audio_to_connection> create_noop();

protected:
  client::logger logger{"add_audio_to_connection"};
  std::shared_ptr<rtc::audio_track> audio_track;
};
} // namespace client

#endif
