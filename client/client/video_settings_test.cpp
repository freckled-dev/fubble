#include "client/add_video_to_connection.hpp"
#include "client/tracks_adder.hpp"
#include "client/video_settings.hpp"
#include "rtc/google/capture/video/device.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
struct mock_video_devices : rtc::video_devices {
  MOCK_METHOD(bool, enumerate, (), (override));
  MOCK_METHOD(std::vector<information>, get_enumerated, (), (const override));
};
struct mock_video_device_factory : rtc::google::capture::video::device_factory {
  MOCK_METHOD(std::unique_ptr<rtc::google::capture::video::device>, create,
              (const std::string &), (override));
};
struct mock_own_video : client::own_video {
  MOCK_METHOD(void, add, (rtc::google::video_source &), (override));
  MOCK_METHOD(void, remove, (rtc::google::video_source &), (override));
  MOCK_METHOD(videos_type, get_all, (), (const override));
};
struct mock_tracks_adder : client::tracks_adder {};
struct mock_add_video_to_connection_factory
    : client::add_video_to_connection_factory {
  MOCK_METHOD(std::unique_ptr<client::add_video_to_connection>, create,
              (const std::shared_ptr<rtc::google::video_source> &), (override));
};
struct VideoSettings : ::testing::Test {
  mock_video_devices devices;
  mock_video_device_factory device_factory;
  mock_own_video own_videos;
  mock_tracks_adder tracks_adder;
  mock_add_video_to_connection_factory add_video_to_connection_factory;
  client::video_settings settings{devices, device_factory, own_videos,
                                  tracks_adder,
                                  add_video_to_connection_factory};
};
} // namespace

TEST_F(VideoSettings, Instance) {}
