#ifndef UUID_709FB41B_8B14_446F_B83F_E736A4371022
#define UUID_709FB41B_8B14_446F_B83F_E736A4371022

#include "rtc/google/video_source_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
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
  boost::signals2::signal<void(const std::string &)> on_name_changed;

  using videos_type = std::vector<rtc::google::video_source *>;
  virtual videos_type get_videos() const = 0;
  boost::signals2::signal<void(const rtc::google::video_source_ptr &)>
      on_video_added;

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
