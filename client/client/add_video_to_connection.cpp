#include "add_video_to_connection.hpp"
#include "rtc/google/connection.hpp"
#include "rtc/google/video_track.hpp"

using namespace client;

add_video_to_connection::add_video_to_connection(
    rtc::google::factory &rtc_factory,
    const std::shared_ptr<rtc::google::video_source> &source)
    : rtc_factory(rtc_factory), source(source) {}

add_video_to_connection::~add_video_to_connection() = default;

void add_video_to_connection::add_to_connection(rtc::connection &connection) {
  std::shared_ptr<rtc::google::video_track> video_track =
      rtc_factory.create_video_track(source);
  connection.add_track(video_track);
}
