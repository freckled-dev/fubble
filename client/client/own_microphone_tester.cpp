#include "own_microphone_tester.hpp"
#include "client/loopback_audio.hpp"

using namespace client;

namespace {
class own_microphone_tester_impl : public own_microphone_tester {
public:
  own_microphone_tester_impl(loopback_audio &audio) : audio(audio) {}

  void start() override { audio.enable_loopback(true); }

  void stop() override { audio.enable_loopback(false); }

  bool get_started() const override { return audio.get_enable_loopback(); }

protected:
  client::logger logger{"own_microphone_tester_impl"};
  loopback_audio &audio;
};
} // namespace

std::unique_ptr<own_microphone_tester>
own_microphone_tester::create(loopback_audio &audio) {
  return std::make_unique<own_microphone_tester_impl>(audio);
}
