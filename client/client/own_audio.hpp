#ifndef UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E
#define UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E

#include "client/logger.hpp"
#include "rtc/track_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace rtc {
class connection;
namespace google {
class factory;
class audio_source;
class audio_track;
class audio_track_sink;
} // namespace google
} // namespace rtc

namespace client {
class own_audio {
public:
  own_audio(rtc::google::factory &rtc_factory);
  ~own_audio();

  void start(rtc::google::audio_source &audio_source);

  void enable_loopback(const bool enable);
  bool get_enable_loopback(const bool enable) const;
  boost::signals2::signal<void(rtc::google::audio_track &)> on_track;
  rtc::google::audio_track *get_track();

protected:
  void negotiation_needed();
  void on_audio_track(rtc::track_ptr track);
  void on_created_connection(boost::future<void> &result);

  client::logger logger{"own_audio"};
  boost::inline_executor executor;
  rtc::google::factory &rtc_factory;
  std::unique_ptr<rtc::connection> rtc_connection_offering;
  std::unique_ptr<rtc::connection> rtc_connection_answering;
  std::shared_ptr<rtc::google::audio_track_sink> audio_track;
  bool enable_audio_loopback_{};
};
} // namespace client

#endif
