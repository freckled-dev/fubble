#ifndef UUID_FD8A9B65_2453_4AB1_A862_41616BC96172
#define UUID_FD8A9B65_2453_4AB1_A862_41616BC96172

#include "client/logger.hpp"
#include "client/track_adder.hpp"
#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/factory.hpp"

namespace client {
class add_audio_to_connection : public track_adder {
public:
  virtual std::shared_ptr<rtc::google::audio_track> get_track() = 0;

  static std::unique_ptr<add_audio_to_connection>
  create(std::shared_ptr<rtc::google::audio_track> source);
  static std::unique_ptr<add_audio_to_connection> create_noop();

protected:
  client::logger logger{"add_audio_to_connection"};
  std::shared_ptr<rtc::google::audio_track> audio_track;
};
} // namespace client

#endif
