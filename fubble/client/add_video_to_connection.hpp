#ifndef UUID_D2EB99EB_418F_4447_BCF0_879740D9D162
#define UUID_D2EB99EB_418F_4447_BCF0_879740D9D162

#include <fubble/client/logger.hpp>
#include <fubble/client/track_adder.hpp>
#include <fubble/rtc/connection.hpp>
#include <fubble/rtc/factory.hpp>
#include <fubble/rtc/video_source.hpp>
#include <fubble/rtc/video_track.hpp>

namespace client {
class add_video_to_connection : public track_adder {
public:
  virtual std::shared_ptr<rtc::video_track> get_video_track() const = 0;
};

class add_video_to_connection_impl : public add_video_to_connection {
public:
  add_video_to_connection_impl(
      std::shared_ptr<rtc::factory> rtc_factory,
      const std::shared_ptr<rtc::video_source> &source);
  ~add_video_to_connection_impl();
  void add_to_connection(rtc::connection &connection) override;
  void remove_from_connection(rtc::connection &connection) override;
  std::shared_ptr<rtc::video_track> get_video_track() const override {
    return video_track;
  }

protected:
  client::logger logger{"add_video_to_connection"};
  std::shared_ptr<rtc::factory> rtc_factory;
  const std::shared_ptr<rtc::video_source> source;
  std::shared_ptr<rtc::video_track> video_track;
};

class add_video_to_connection_noop : public add_video_to_connection {
public:
  void add_to_connection(rtc::connection &) override {}
  void remove_from_connection(rtc::connection &) override {}
  std::shared_ptr<rtc::video_track> get_video_track() const override {
    return {};
  }
};

class add_video_to_connection_factory {
public:
  virtual ~add_video_to_connection_factory() = default;
  virtual std::unique_ptr<add_video_to_connection>
  create(const std::shared_ptr<rtc::video_source> &source) = 0;
};
class add_video_to_connection_factory_impl
    : public add_video_to_connection_factory {
public:
  add_video_to_connection_factory_impl(
      std::shared_ptr<rtc::factory> rtc_factory);

  virtual std::unique_ptr<add_video_to_connection>
  create(const std::shared_ptr<rtc::video_source> &source) override;

  std::shared_ptr<rtc::factory> rtc_factory;
};
} // namespace client

#endif
