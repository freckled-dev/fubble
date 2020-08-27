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
} // namespace

TEST(VideoSettings, Instance) {
#if 0
      rtc::video_devices &enumerator,
      rtc::google::capture::video::device_factory &device_creator,
      own_media &own_media_, tracks_adder &tracks_adder_,
      add_video_to_connection_factory &add_video_to_connection_factory_);
#endif
  mock_video_devices devices;
  mock_video_device_factory device_factory;
  // client::video_settings settings{video_devices};
}
