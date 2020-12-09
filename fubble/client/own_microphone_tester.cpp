#include "own_microphone_tester.hpp"
#include "fubble/client/audio_tracks_volume.hpp"
#include "fubble/client/loopback_audio.hpp"

using namespace client;

namespace {
class own_microphone_tester_impl : public own_microphone_tester {
public:
  own_microphone_tester_impl(loopback_audio &audio,
                             audio_tracks_volume &audio_volume)
      : audio(audio), audio_volume(audio_volume) {}

  void start() override {
    audio.enable_loopback(true);
    const bool already_deafend = audio_volume.get_deafen();
    if (already_deafend)
      return;
    audio_volume.deafen(true);
    did_deafen = true;
  }

  void stop() override {
    audio.enable_loopback(false);
    if (!did_deafen)
      return;
    audio_volume.deafen(false);
    did_deafen = false;
  }

  bool get_started() const override { return audio.get_enable_loopback(); }

protected:
  client::logger logger{"own_microphone_tester_impl"};
  loopback_audio &audio;
  audio_tracks_volume &audio_volume;
  bool did_deafen{};
};
} // namespace

std::unique_ptr<own_microphone_tester>
own_microphone_tester::create(loopback_audio &audio,
                              audio_tracks_volume &audio_volume) {
  return std::make_unique<own_microphone_tester_impl>(audio, audio_volume);
}
