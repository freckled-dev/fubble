#ifndef UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E
#define UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E

#include "client/logger.hpp"
#include "rtc/track_ptr.hpp"
#include <boost/thread/executors/inline_executor.hpp>

namespace rtc::google {
class factory;
class connection;
class audio_source;
class audio_track_sink;
namespace capture::audio {
class device;
}
} // namespace rtc::google

namespace client {
class audio_level_calculator;
class own_audio {
public:
  own_audio(rtc::google::factory &rtc_factory,
            rtc::google::audio_source &audio_source);
  ~own_audio();

  void enable_audio_loopback(const bool enable);
  bool get_enable_audio_loopback(const bool enable) const;

protected:
  void negotiation_needed();
  void on_audio_track(rtc::track_ptr track);

  client::logger logger{"own_audio"};
  boost::inline_executor executor;
  rtc::google::factory &rtc_factory;
  std::unique_ptr<rtc::google::connection> rtc_connection_offering;
  std::unique_ptr<rtc::google::connection> rtc_connection_answering;
  std::unique_ptr<audio_level_calculator> audio_level_calculator_;
  std::shared_ptr<rtc::google::audio_track_sink> audio_track;
  bool enable_audio_loopback_{};
};
} // namespace client

#endif
