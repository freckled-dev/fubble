#ifndef UUID_FD8A9B65_2453_4AB1_A862_41616BC96172
#define UUID_FD8A9B65_2453_4AB1_A862_41616BC96172

#include "client/logger.hpp"
#include "client/track_adder.hpp"
#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/factory.hpp"

namespace client {
class add_audio_to_connection : public track_adder {
public:
  add_audio_to_connection(std::shared_ptr<rtc::google::audio_track> source);
  ~add_audio_to_connection();
  void add_to_connection(rtc::connection &connection) override;
  void remove_from_connection(rtc::connection &connection) override;

  std::shared_ptr<rtc::google::audio_track> get_track();

protected:
  client::logger logger{"add_audio_to_connection"};
  std::shared_ptr<rtc::google::audio_track> audio_track;
};
} // namespace client

#endif
