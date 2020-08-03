#include "loopback_audio.hpp"
#include "client/add_audio_to_connection.hpp"
#include "rtc/google/audio_source.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/audio_track_sink.hpp"
#include "rtc/google/connection.hpp"
#include "rtc/google/factory.hpp"

using namespace client;

namespace {
void connect_ice_signal(rtc::connection &from, rtc::connection &to) {
  from.on_ice_candidate.connect(
      [&to](auto candidate) { to.add_ice_candidate(candidate); });
}
} // namespace

loopback_audio_impl::loopback_audio_impl(
    rtc::google::factory &rtc_factory,
    std::shared_ptr<rtc::google::audio_track> audio_source)
    : rtc_factory(rtc_factory) {
  rtc_connection_offering = rtc_factory.create_connection();
  rtc_connection_answering = rtc_factory.create_connection();
  connect_ice_signal(*rtc_connection_offering, *rtc_connection_answering);
  connect_ice_signal(*rtc_connection_answering, *rtc_connection_offering);
  std::shared_ptr<rtc::google::audio_track> sending_audio_track = audio_source;
  BOOST_ASSERT(sending_audio_track);
#if 1
  rtc_connection_offering->on_negotiation_needed.connect(
      [this] { negotiation_needed(); });
#endif
#if 1
  rtc_connection_answering->on_audio_track_added.connect(
      [this](rtc::track_ptr track) { on_audio_track(track); });
#endif
#if 1
  rtc_connection_offering->add_track(sending_audio_track);
#endif
  // sending_audio_track->set_enabled(false);
}

void loopback_audio_impl::negotiation_needed() {
  auto negotiated =
      rtc_connection_offering->create_offer()
          .then(executor,
                [this](auto offer) {
                  auto got_offer = offer.get();
                  rtc_connection_offering->set_local_description(got_offer);
                  return rtc_connection_answering->set_remote_description(
                      got_offer);
                })
          .unwrap()
          .then(executor,
                [this](auto future) {
                  future.get();
                  return rtc_connection_answering->create_answer();
                })
          .unwrap()
          .then(executor,
                [this](auto answer) {
                  auto got_answer = answer.get();
                  rtc_connection_answering->set_local_description(got_answer);
                  return rtc_connection_offering->set_remote_description(
                      got_answer);
                })
          .unwrap();
#if 1
  negotiated.then(executor,
                  [this](auto result) { on_created_connection(result); });
#endif
}

loopback_audio_impl::~loopback_audio_impl() = default;

void loopback_audio_impl::enable_loopback(const bool enable) {
  if (enable_audio_loopback_ == enable)
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", enable:" << enable;
  enable_audio_loopback_ = enable;
  if (!audio_track)
    return;
  audio_track->set_enabled(enable_audio_loopback_);
}
bool loopback_audio_impl::get_enable_loopback() const {
  return enable_audio_loopback_;
}

std::shared_ptr<rtc::google::audio_track> loopback_audio_impl::get_track() {
  return audio_track;
}

void loopback_audio_impl::on_audio_track(rtc::track_ptr track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  BOOST_ASSERT(!audio_track);
  track->set_enabled(enable_audio_loopback_);
  audio_track = std::dynamic_pointer_cast<rtc::google::audio_track_sink>(track);
  on_track(*audio_track);
}

void loopback_audio_impl::on_created_connection(boost::future<void> &result) {
  try {
    result.get();
    BOOST_LOG_SEV(logger, logging::severity::debug) << "negotiated";
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error) << "negotiation, failed";
  }
}

loopback_audio_impl_factory::loopback_audio_impl_factory(
    rtc::google::factory &rtc_factory,
    std::shared_ptr<rtc::google::audio_track> audio_source)
    : rtc_factory(rtc_factory), audio_source(audio_source) {}

std::unique_ptr<loopback_audio> loopback_audio_impl_factory::create() {
  return std::make_unique<loopback_audio_impl>(rtc_factory, audio_source);
}

std::unique_ptr<loopback_audio> loopback_audio_noop_factory::create() {
  return std::make_unique<loopback_audio_noop>();
}

loopback_audio_noop_if_disabled::loopback_audio_noop_if_disabled(
    loopback_audio_factory &factory)
    : factory(factory) {}

loopback_audio_noop_if_disabled::~loopback_audio_noop_if_disabled() = default;

void loopback_audio_noop_if_disabled::enable_loopback(const bool enable) {
  if (get_enable_loopback() == enable) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << ", already enabled/disabled, enable:" << enable;
    return;
  }
  if (!enable) {
    BOOST_ASSERT(delegate);
    delegate.reset();
    return;
  }
  BOOST_ASSERT(!delegate);
  delegate = factory.create();
  delegate->enable_loopback(enable);
  delegate->on_track.connect([this](auto &track) { on_track(track); });
}

bool loopback_audio_noop_if_disabled::get_enable_loopback() const {
  return delegate != nullptr;
}

std::shared_ptr<rtc::google::audio_track>
loopback_audio_noop_if_disabled::get_track() {
  if (!delegate)
    return nullptr;
  return delegate->get_track();
}
