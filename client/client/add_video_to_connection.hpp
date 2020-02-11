#ifndef UUID_D2EB99EB_418F_4447_BCF0_879740D9D162
#define UUID_D2EB99EB_418F_4447_BCF0_879740D9D162

#include "rtc/connection.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/factory.hpp"
#include "track_adder.hpp"

namespace client {
class add_video_to_connection : public track_adder {
public:
  add_video_to_connection(
      rtc::google::factory &rtc_factory,
      const std::shared_ptr<rtc::google::video_source> &source);
  ~add_video_to_connection();
  void add_to_connection(rtc::connection &connection) override;

protected:
  rtc::google::factory &rtc_factory;
  const std::shared_ptr<rtc::google::video_source> source;
};
} // namespace client

#endif
