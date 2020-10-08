#include "add_audio_to_connection.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/connection.hpp"

using namespace client;

namespace {
class add_audio_to_connection_noop final : public add_audio_to_connection {
public:
  void add_to_connection(rtc::connection &) override {}
  void remove_from_connection(rtc::connection &) override {}
  std::shared_ptr<rtc::google::audio_track> get_track() override { return {}; }
};
class add_audio_to_connection_impl final : public add_audio_to_connection {
public:
  add_audio_to_connection_impl(std::shared_ptr<rtc::google::audio_track> source)
      : audio_track(source) {}

  void add_to_connection(rtc::connection &connection) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    connection.add_track(audio_track);
  }

  void remove_from_connection(rtc::connection &connection) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    connection.remove_track(audio_track);
  }

  std::shared_ptr<rtc::google::audio_track> get_track() override {
    return audio_track;
  }

protected:
  client::logger logger{"add_audio_to_connection"};
  std::shared_ptr<rtc::google::audio_track> audio_track;
};
} // namespace

std::unique_ptr<add_audio_to_connection> add_audio_to_connection::create(
    std::shared_ptr<rtc::google::audio_track> source) {
  return std::make_unique<add_audio_to_connection_impl>(source);
}

std::unique_ptr<add_audio_to_connection>
add_audio_to_connection::create_noop() {
  return std::make_unique<add_audio_to_connection_noop>();
}
