#include "add_video_to_connection.hpp"
#include "rtc/google/connection.hpp"
#include "rtc/google/video_track.hpp"

using namespace client;

add_video_to_connection::add_video_to_connection(
    const std::shared_ptr<rtc::google::video_source> &source)
    : source(source) {}
void own_video::add_to_connection(rtc::connection &connection) {
  std::shared_ptr<rtc::google::video_track> video_track =
      rtc_connection_creator.create_video_track(capture_device);
  connection.add_track(track);
}

