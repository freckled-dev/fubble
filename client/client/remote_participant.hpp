#ifndef UUID_879851E1_6012_47BE_AF61_ECB95660B186
#define UUID_879851E1_6012_47BE_AF61_ECB95660B186

#include "client/logger.hpp"
#include "participant.hpp"
#include "rtc/track_ptr.hpp"

namespace rtc::google {
class audio_track_sink;
}

namespace client {
class peer;
class peer_creator;
class factory;
class audio_track_information_listener;
class remote_participant : public participant {
public:
  remote_participant(factory &factory_, std::unique_ptr<peer> peer_,
                     matrix::user &matrix_participant);
  ~remote_participant();

  boost::future<void> close() override;

protected:
  videos_type get_videos() const override;
  void on_track(rtc::track_ptr track);
  void
  on_audio_track(std::shared_ptr<rtc::google::audio_track_sink> audio_track);
  void on_video_track(rtc::google::video_source_ptr video_track);

  client::logger logger{"remote_participant"};
  factory &factory_;
  std::unique_ptr<audio_track_information_listener> audio_information;
  std::unique_ptr<peer> peer_;
  videos_type videos;
};
} // namespace client

#endif
