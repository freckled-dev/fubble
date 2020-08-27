#ifndef UUID_D2EB99EB_418F_4447_BCF0_879740D9D162
#define UUID_D2EB99EB_418F_4447_BCF0_879740D9D162

#include "client/logger.hpp"
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
  void remove_from_connection(rtc::connection &connection) override;

protected:
  client::logger logger{"add_video_to_connection"};
  rtc::google::factory &rtc_factory;
  const std::shared_ptr<rtc::google::video_source> source;
  std::shared_ptr<rtc::google::video_track> video_track;
};
class add_video_to_connection_factory {
public:
  add_video_to_connection_factory(rtc::google::factory &rtc_factory);
  virtual ~add_video_to_connection_factory() = default;

  virtual std::unique_ptr<add_video_to_connection>
  create(const std::shared_ptr<rtc::google::video_source> &source);

  rtc::google::factory &rtc_factory;
};
} // namespace client

#endif
