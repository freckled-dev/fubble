#ifndef UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E
#define UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E

#include "client/logger.hpp"
#include "rtc/track_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace rtc {
class connection;
}
namespace rtc::google {
class audio_track;
class audio_track_sink;
class factory;
} // namespace rtc::google

namespace client {
class loopback_audio {
public:
  virtual ~loopback_audio() = default;

  virtual void enable_loopback(const bool enable) = 0;
  virtual bool get_enable_loopback() const = 0;
  virtual rtc::google::audio_track &get_track() = 0;

  boost::signals2::signal<void(rtc::google::audio_track &)> on_track;
};

class loopback_audio_noop : public loopback_audio {
  void enable_loopback(const bool enable) override {}
  bool get_enable_loopback() const override { return false; }
  rtc::google::audio_track &get_track() override { BOOST_ASSERT(false); }
};

class loopback_audio_impl : public loopback_audio {
public:
  loopback_audio_impl(rtc::google::factory &rtc_factory,
                      std::shared_ptr<rtc::google::audio_track> audio_source);
  ~loopback_audio_impl();

  void enable_loopback(const bool enable) override;
  bool get_enable_loopback() const override;
  rtc::google::audio_track &get_track() override;

protected:
  void negotiation_needed();
  void on_audio_track(rtc::track_ptr track);
  void on_created_connection(boost::future<void> &result);

  client::logger logger{"loopback_audio"};
  boost::inline_executor executor;
  rtc::google::factory &rtc_factory;
  std::unique_ptr<rtc::connection> rtc_connection_offering;
  std::unique_ptr<rtc::connection> rtc_connection_answering;
  std::shared_ptr<rtc::google::audio_track_sink> audio_track;
  bool enable_audio_loopback_{false}; // set to true to default hear yourself
};
class loopback_audio_factory {
public:
  virtual ~loopback_audio_factory() = default;
  virtual std::unique_ptr<loopback_audio> create() = 0;
};
class loopback_audio_impl_factory : public loopback_audio_factory {
public:
  loopback_audio_impl_factory(
      rtc::google::factory &rtc_factory,
      std::shared_ptr<rtc::google::audio_track> audio_source);
  std::unique_ptr<loopback_audio> create() override;

protected:
  rtc::google::factory &rtc_factory;
  std::shared_ptr<rtc::google::audio_track> audio_source;
};
class loopback_audio_noop_factory : public loopback_audio_factory {
public:
  std::unique_ptr<loopback_audio> create() override;
};
} // namespace client

#endif
