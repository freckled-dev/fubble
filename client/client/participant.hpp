#ifndef UUID_709FB41B_8B14_446F_B83F_E736A4371022
#define UUID_709FB41B_8B14_446F_B83F_E736A4371022

#include "rtc/google/video_source_ptr.hpp"
#include "session/participant.hpp"
#include <boost/signals2/signal.hpp>
#include <memory>

namespace client {
class participant {
public:
  virtual ~participant();

  virtual std::string get_id() const = 0;
  virtual std::string get_name() const = 0;
  virtual void update(const session::participant &update) = 0;

  boost::signals2::signal<void(const rtc::google::video_source_ptr &)>
      on_video_added;
  boost::signals2::signal<void(const std::string &)> on_name_changed;
};
} // namespace client

#endif
