#include "add_video_to_connection.hpp"
#include "fubble/rtc/google/connection.hpp"
#include "fubble/rtc/google/video_track.hpp"

using namespace client;

add_video_to_connection_impl::add_video_to_connection_impl(
    rtc::google::factory &rtc_factory,
    const std::shared_ptr<rtc::google::video_source> &source)
    : rtc_factory(rtc_factory), source(source) {
  video_track = rtc_factory.create_video_track(source);
}

add_video_to_connection_impl::~add_video_to_connection_impl() = default;

void add_video_to_connection_impl::add_to_connection(
    rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  connection.add_track(video_track);
}

void add_video_to_connection_impl::remove_from_connection(
    rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  connection.remove_track(video_track);
}

add_video_to_connection_factory_impl::add_video_to_connection_factory_impl(
    rtc::google::factory &rtc_factory)
    : rtc_factory(rtc_factory) {}

std::unique_ptr<add_video_to_connection>
add_video_to_connection_factory_impl::create(
    const std::shared_ptr<rtc::google::video_source> &source) {
  return std::make_unique<client::add_video_to_connection_impl>(rtc_factory,
                                                                source);
}
