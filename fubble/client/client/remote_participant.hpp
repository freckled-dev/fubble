#ifndef UUID_879851E1_6012_47BE_AF61_ECB95660B186
#define UUID_879851E1_6012_47BE_AF61_ECB95660B186

#include "client/logger.hpp"
#include "participant.hpp"
#include "fubble/rtc/track_ptr.hpp"

namespace rtc::google {
class audio_track_sink;
} // namespace rtc::google

namespace client {
class audio_track_information_listener;
class factory;
class peer;
class peer_creator;
class tracks_adder;
class remote_participant : public participant {
public:
  remote_participant(factory &factory_, std::unique_ptr<peer> peer_,
                     matrix::user &matrix_participant,
                     tracks_adder &tracks_adder_);
  ~remote_participant();

  boost::future<void> close() override;

protected:
  videos_type get_videos() const override;
  audios_type get_audios() const override;
  void on_track_added(rtc::track_ptr track);
  void on_track_removed(rtc::track_ptr track);
  void on_video_track(rtc::google::video_source_ptr video_track);
  void on_video_track_removed(rtc::google::video_source_ptr video_track);
  void
  on_audio_track(std::shared_ptr<rtc::google::audio_track_sink> audio_track);
  void on_audio_track_removed(
      std::shared_ptr<rtc::google::audio_track_sink> video_track);

  client::logger logger;
  factory &factory_;
  tracks_adder &tracks_adder_;
  std::unique_ptr<peer> peer_;
  videos_type videos;
  audios_type audios;
};
} // namespace client

#endif
