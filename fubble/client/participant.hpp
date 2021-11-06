#ifndef UUID_709FB41B_8B14_446F_B83F_E736A4371022
#define UUID_709FB41B_8B14_446F_B83F_E736A4371022

#include <fubble/utils/signal.hpp>
#include <boost/thread/future.hpp>
#include <fubble/rtc/audio_track.hpp>
#include <fubble/rtc/video_source.hpp>
#include <memory>

namespace matrix {
class user;
}

namespace client {
class participant {
public:
  participant(matrix::user &matrix_participant);
  virtual ~participant();

  virtual boost::future<void> close() = 0;
  std::string get_id() const;
  std::string get_name() const;
  utils::signal::signal<const std::string &> on_name_changed;

  using video_ptr = std::shared_ptr<rtc::video_source>;
  using videos_type = std::vector<video_ptr>;
  virtual videos_type get_videos() const = 0;
  utils::signal::signal<video_ptr> on_video_added;
  utils::signal::signal<video_ptr> on_video_removed;

  // when refactoring to shared_ptr, ensure
  // `audios.emplace_back(audio_track.get());` don't happen
  using audios_type = std::vector<rtc::audio_track *>;
  virtual audios_type get_audios() const = 0;
  utils::signal::signal<rtc::audio_track &> on_audio_added;
  utils::signal::signal<rtc::audio_track &> on_audio_removed;

  virtual videos_type get_screens() const { return {}; }
  utils::signal::signal<video_ptr> on_screen_added;
  utils::signal::signal<video_ptr> on_screen_removed;

protected:
  void update();

  matrix::user &matrix_participant;
  // on a delete the matrix_participant gets delted before the signal
  // `on_matrix_participant_leaves` gets called
  const std::string id;
  utils::signal::scoped_connection connection_update;
};
} // namespace client

#endif
