#ifndef UUID_FD8A9B65_2453_4AB1_A862_41616BC96172
#define UUID_FD8A9B65_2453_4AB1_A862_41616BC96172

#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/factory.hpp"
#include "track_adder.hpp"

namespace client {
class add_audio_to_connection : public track_adder {
public:
  add_audio_to_connection(rtc::google::factory &rtc_factory,
                          rtc::google::audio_source &source);
  ~add_audio_to_connection();
  void add_to_connection(rtc::connection &connection) override;

protected:
  rtc::google::factory &rtc_factory;
  rtc::google::audio_source &source;
};
} // namespace client

#endif
