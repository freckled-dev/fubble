#ifndef UUID_6D858043_CB01_45E3_A23A_B270D61299A8
#define UUID_6D858043_CB01_45E3_A23A_B270D61299A8

#include "client/logger.hpp"
#include "utils/interval_timer.hpp"

namespace rtc::google {
class audio_track;
}

namespace client {
class audio_track_information_listener {
public:
  audio_track_information_listener(boost::asio::io_context &context,
                                   rtc::google::audio_track &track);

protected:
  void update();

  client::logger logger{"audio_track_information_listener"};
  boost::asio::io_context &context;
  rtc::google::audio_track &track;
  std::chrono::steady_clock::duration pull_interval = std::chrono::seconds{1};
  utils::interval_timer timer{context, pull_interval};
  bool voice_detected{};
  int rms_dbfs{};
};
} // namespace client

#endif
