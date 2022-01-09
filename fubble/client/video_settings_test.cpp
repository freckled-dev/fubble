#include <fubble/client/add_video_to_connection.hpp>
#include <fubble/client/tracks_adder.hpp>
#include <fubble/client/video_settings.hpp>
#include <fubble/rtc/google/capture/video/device.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
struct mock_video_devices : rtc::video_devices {
  MOCK_METHOD(bool, enumerate, (), (override));
  MOCK_METHOD(std::vector<information>, get_enumerated, (), (const override));
};
struct mock_video_device_factory : rtc::google::capture::video::device_factory {
  MOCK_METHOD(std::unique_ptr<rtc::video_device>, create, (const std::string &),
              (override));
};
struct mock_video_device : rtc::google::capture::video::device {
  MOCK_METHOD(void, start, (const rtc::video::capability &), (override));
  MOCK_METHOD(void, stop, (), (override));
  MOCK_METHOD(bool, get_started, (), (const override));
  MOCK_METHOD(std::string, get_id, (), (const override));
  MOCK_METHOD(std::shared_ptr<rtc::video_source>, get_source, (),
              (const override));
};
struct mock_own_video : client::own_video {
  MOCK_METHOD(void, add, (std::shared_ptr<rtc::video_source>), (override));
  MOCK_METHOD(void, remove, (std::shared_ptr<rtc::video_source>), (override));
  MOCK_METHOD(videos_type, get_all, (), (const override));
};
struct mock_tracks_adder : client::tracks_adder {};
struct mock_add_video_to_connection_factory
    : client::add_video_to_connection_factory {
  MOCK_METHOD(std::unique_ptr<client::add_video_to_connection>, create,
              (const std::shared_ptr<rtc::video_source> &), (override));
};
struct VideoSettings : ::testing::Test {
  mock_video_devices devices;
  std::shared_ptr<mock_video_device_factory> device_factory =
      std::make_shared<mock_video_device_factory>();
  mock_own_video own_videos;
  mock_tracks_adder tracks_adder;
  mock_add_video_to_connection_factory add_video_to_connection_factory;

  std::unique_ptr<client::video_settings> make_settings() {
    return std::make_unique<client::video_settings>(
        devices, device_factory, own_videos, tracks_adder,
        add_video_to_connection_factory, rtc::video::capability{});
  }

  VideoSettings() {
    ON_CALL(*device_factory, create(::testing::_)).WillByDefault([](auto) {
      return std::make_unique<mock_video_device>();
    });
    ON_CALL(add_video_to_connection_factory, create(::testing::_))
        .WillByDefault([](auto) {
          return std::make_unique<client::add_video_to_connection_noop>();
        });
  }
};
rtc::video_devices::information make_video_device_informatin(std::string id,
                                                             std::string name) {
  rtc::video_devices::information result;
  result.id = id;
  result.name = name;
  return result;
}
} // namespace

TEST_F(VideoSettings, Instance) {
  auto settings = make_settings();
  EXPECT_FALSE(settings->get_device_id());
  EXPECT_FALSE(settings->get_paused());
  EXPECT_FALSE(settings->is_a_video_available());
}

TEST_F(VideoSettings, OneCamera) {
  ON_CALL(devices, get_enumerated()).WillByDefault([] {
    std::vector<rtc::video_devices::information> result;
    result.push_back(make_video_device_informatin("id", "name"));
    return result;
  });
  auto settings = make_settings();
  EXPECT_FALSE(settings->get_paused());
  EXPECT_TRUE(settings->is_a_video_available());
  ASSERT_EQ(settings->get_device_id().value_or("no id"), "id");
}

TEST_F(VideoSettings, AddCamera) {
  auto settings = make_settings();
  ON_CALL(devices, get_enumerated()).WillByDefault([] {
    std::vector<rtc::video_devices::information> result;
    result.push_back(make_video_device_informatin("id", "name"));
    return result;
  });
  devices.on_enumerated_changed();
  EXPECT_FALSE(settings->get_device_id());
  EXPECT_FALSE(settings->get_paused());
  EXPECT_FALSE(settings->is_a_video_available());
}

TEST_F(VideoSettings, PlugOut) {
  ON_CALL(devices, get_enumerated()).WillByDefault([] {
    std::vector<rtc::video_devices::information> result;
    result.push_back(make_video_device_informatin("id", "name"));
    return result;
  });
  auto settings = make_settings();
  ON_CALL(devices, get_enumerated()).WillByDefault([] {
    return std::vector<rtc::video_devices::information>();
  });
  devices.on_enumerated_changed();
  EXPECT_FALSE(settings->get_device_id());
  EXPECT_FALSE(settings->get_paused());
  EXPECT_FALSE(settings->is_a_video_available());
}
