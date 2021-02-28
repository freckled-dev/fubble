#include "video_device.hpp"
#include <fubble/rtc/logger.hpp>
#include <fubble/rtc/video_source.hpp>

using namespace rtc;

namespace {
class FUBBLE_PUBLIC video_device_noop_impl : public video_device_noop {
public:
  video_device_noop_impl(const std::string &id) : id{id} {}

  void start(const rtc::video::capability &cap) override {
    (void)cap;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    started = true;
  }

  void stop() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    started = false;
  }

  bool get_started() const override { return started; }

  std::shared_ptr<rtc::video_source> get_source() const override {
    return source;
  }

  std::string get_id() const override { return id; }

private:
  rtc::logger logger{"video_device_noop"};
  const std::string id;
  bool started{};
  std::shared_ptr<video_source> source = std::make_shared<video_source_noop>();
};
} // namespace

std::unique_ptr<video_device> video_device_noop::create(const std::string &id) {
  return std::make_unique<video_device_noop_impl>(id);
}
