#ifndef UUID_B7301BB0_0919_4201_BB52_DB7FB8CC58F1
#define UUID_B7301BB0_0919_4201_BB52_DB7FB8CC58F1

#include "rtc/logger.hpp"
#include <boost/signals2/signal.hpp>

namespace rtc {
namespace google {
class audio_source;
class audio_source_stats_collector {
public:
  audio_source_stats_collector(audio_source &source);
  ~audio_source_stats_collector();

  void start();
  void stop();

  boost::signals2::signal<void(double)> on_sound_level;

protected:
  rtc::logger logger{"audio_source_stats_collector"};
  audio_source &source;
  struct collector;
  std::unique_ptr<collector> collector_;
};
} // namespace google
} // namespace rtc

#endif
