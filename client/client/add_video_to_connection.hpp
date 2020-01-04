#ifndef UUID_D2EB99EB_418F_4447_BCF0_879740D9D162
#define UUID_D2EB99EB_418F_4447_BCF0_879740D9D162

#include "rtc/connection.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/factory.hpp"

namespace client {
class add_video_to_connection {
public:
  add_video_to_connection(
      rtc::google::factory &rtc_factory,
      const std::shared_ptr<rtc::google::video_source> &source);
  void add_to_connection(rtc::connection &connection);

protected:
  rtc::google::factory &rtc_factory;
  const std::shared_ptr<rtc::google::video_source> source;
};
} // namespace client

#endif
