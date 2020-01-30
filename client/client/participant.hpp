#ifndef UUID_709FB41B_8B14_446F_B83F_E736A4371022
#define UUID_709FB41B_8B14_446F_B83F_E736A4371022

#include "rtc/google/video_source_ptr.hpp"
#include "rtc/track_ptr.hpp"
#include "session/participant.hpp"
#include <boost/signals2/signal.hpp>
#include <memory>

namespace client {
class peer;
class peer_creator;
class participant {
public:
  participant(std::unique_ptr<peer> peer_,
              const session::participant &session_participant);
  ~participant();

  std::string get_name() const;

  boost::signals2::signal<void(const rtc::google::video_source_ptr &)>
      on_video_added;

protected:
  void on_track(rtc::track_ptr track);

  std::unique_ptr<peer> peer_;
  session::participant session_participant;
};
} // namespace client

#endif
