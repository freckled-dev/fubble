#ifndef CLIENT_OWN_VIDEO_HPP
#define CLIENT_OWN_VIDEO_HPP

#include "rtc/connection.hpp"
#include "rtc/google/capture/video/device.hpp"

namespace client {
class own_video {
public:
  add_video_to_connection(
      const std::shared_ptr<rtc::google::video_source> &source);
  void add_to_connection(rtc::connection &connection);

protected:
  const std::shared_ptr<rtc::google::video_source> source;
};
} // namespace client

#endif
