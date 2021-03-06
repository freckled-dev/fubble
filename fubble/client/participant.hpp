#ifndef UUID_709FB41B_8B14_446F_B83F_E736A4371022
#define UUID_709FB41B_8B14_446F_B83F_E736A4371022

#include "fubble/rtc/google/video_source_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
#include <memory>

namespace rtc::google {
class audio_track;
}
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
  boost::signals2::signal<void(const std::string &)> on_name_changed;

  using video_ptr = std::shared_ptr<rtc::google::video_source>;
  using videos_type = std::vector<video_ptr>;
  virtual videos_type get_videos() const = 0;
  boost::signals2::signal<void(video_ptr)> on_video_added;
  boost::signals2::signal<void(video_ptr)> on_video_removed;

  // when refactoring to shared_ptr, ensure
  // `audios.emplace_back(audio_track.get());` don't happen
  using audios_type = std::vector<rtc::google::audio_track *>;
  virtual audios_type get_audios() const = 0;
  boost::signals2::signal<void(rtc::google::audio_track &)> on_audio_added;
  boost::signals2::signal<void(rtc::google::audio_track &)> on_audio_removed;

  virtual videos_type get_screens() const { return {}; }
  boost::signals2::signal<void(video_ptr)> on_screen_added;
  boost::signals2::signal<void(video_ptr)> on_screen_removed;

protected:
  void update();

  matrix::user &matrix_participant;
  // on a delete the matrix_participant gets delted before the signal
  // `on_matrix_participant_leaves` gets called
  const std::string id;
  boost::signals2::scoped_connection connection_update;
};
} // namespace client

#endif
