#include "rtc/connection.hpp"

namespace rtc::gstreamer {
class connection : public rtc::connection {
public:
  connection();
  ~connection() override;
  boost::future<session_description> create_offer() override;
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;
  void add_track(track_ptr) override;
};
} // namespace rtc::gstreamer
