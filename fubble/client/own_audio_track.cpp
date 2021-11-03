#include "own_audio_track.hpp"
#include "fubble/rtc/audio_track.hpp"
#include "fubble/rtc/factory.hpp"

using namespace client;

namespace {
struct own_audio_track_impl : own_audio_track {
  own_audio_track_impl(rtc::factory &rtc_factory,
                       rtc::audio_source &audio_source) {
    audio_track = rtc_factory.create_audio_track(audio_source);
  }

  void set_enabled(bool enabled) override { audio_track->set_enabled(enabled); }

  std::shared_ptr<rtc::audio_track> get_track() override { return audio_track; }

protected:
  std::shared_ptr<rtc::audio_track> audio_track;
};
struct own_audio_track_noop : own_audio_track {
  own_audio_track_noop() {}

  void set_enabled(bool) override{};

  std::shared_ptr<rtc::audio_track> get_track() override {
    BOOST_ASSERT(false);
    return nullptr;
  }
};
} // namespace

std::unique_ptr<own_audio_track>
own_audio_track::create(rtc::factory &rtc_factory,
                        rtc::audio_source &audio_source) {
  return std::make_unique<own_audio_track_impl>(rtc_factory, audio_source);
}

std::unique_ptr<own_audio_track> own_audio_track::create_noop() {
  return std::make_unique<own_audio_track_noop>();
}
