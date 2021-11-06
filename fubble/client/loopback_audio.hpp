#ifndef UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E
#define UUID_B5FA71A7_53B9_4F85_BEA4_05ED7D9F580E

#include "fubble/client/logger.hpp"
#include "fubble/rtc/track_ptr.hpp"
#include <sigslot/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace rtc {
class connection;
class audio_track;
class factory;
} // namespace rtc

namespace client {
class loopback_audio {
public:
  virtual ~loopback_audio() = default;

  virtual void enable_loopback(const bool enable) = 0;
  virtual bool get_enable_loopback() const = 0;
  virtual std::shared_ptr<rtc::audio_track> get_track() = 0;

  sigslot::signal<rtc::audio_track &> on_track;

  static std::unique_ptr<loopback_audio>
  create(rtc::factory &rtc_factory,
         std::shared_ptr<rtc::audio_track> audio_source,
         std::shared_ptr<boost::executor> defering_executor);
};

class loopback_audio_noop : public loopback_audio {
  void enable_loopback(const bool) override {}
  bool get_enable_loopback() const override { return false; }
  std::shared_ptr<rtc::audio_track> get_track() override {
    BOOST_ASSERT(false);
    return {};
  }
};

class loopback_audio_factory {
public:
  virtual ~loopback_audio_factory() = default;
  virtual std::unique_ptr<loopback_audio> create() = 0;
};
class loopback_audio_impl_factory : public loopback_audio_factory {
public:
  loopback_audio_impl_factory(
      rtc::factory &rtc_factory, std::shared_ptr<rtc::audio_track> audio_source,
      std::shared_ptr<boost::executor> defering_executor);
  std::unique_ptr<loopback_audio> create() override;

protected:
  rtc::factory &rtc_factory;
  std::shared_ptr<rtc::audio_track> audio_source;
  std::shared_ptr<boost::executor> defering_executor;
};
class loopback_audio_noop_factory : public loopback_audio_factory {
public:
  std::unique_ptr<loopback_audio> create() override;
};
class loopback_audio_noop_if_disabled : public loopback_audio {
public:
  loopback_audio_noop_if_disabled(loopback_audio_factory &factory);
  ~loopback_audio_noop_if_disabled();

  void enable_loopback(const bool enable) override;
  bool get_enable_loopback() const override;

  std::shared_ptr<rtc::audio_track> get_track() override;

protected:
  client::logger logger{"loopback_audio_noop_if_disabled"};
  loopback_audio_factory &factory;
  std::unique_ptr<loopback_audio> delegate;
};
} // namespace client

#endif
