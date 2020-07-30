#include "own_microphone_tester.hpp"
#include "client/loopback_audio.hpp"

using namespace client;

namespace {
class own_microphone_tester_impl : public own_microphone_tester {
public:
  own_microphone_tester_impl(loopback_audio_factory &loopback_audio_factory_)
      : loopback_audio_factory_(loopback_audio_factory_) {}

  void start() override {
    if (loopback) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", already started";
      BOOST_ASSERT(false);
      return;
    }
    loopback = loopback_audio_factory_.create();
    loopback->enable_loopback(true);
  }

  void stop() override {
    if (!loopback) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", not started";
      BOOST_ASSERT(false);
      return;
    }
    loopback.reset();
  }

  bool get_started() const override { return loopback != nullptr; }

protected:
  client::logger logger{"own_microphone_tester_impl"};
  loopback_audio_factory &loopback_audio_factory_;
  std::unique_ptr<loopback_audio> loopback;
};
} // namespace

std::unique_ptr<own_microphone_tester>
own_microphone_tester::create(loopback_audio_factory &loopback_audio_factory_) {
  return std::make_unique<own_microphone_tester_impl>(loopback_audio_factory_);
}
