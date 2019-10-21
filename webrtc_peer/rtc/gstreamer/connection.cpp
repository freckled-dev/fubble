#include "connection.hpp"
extern "C" {
#include <gst/gst.h>
#include <gst/gstpromise.h>
#include <gst/sdp/sdp.h>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

using namespace rtc::gstreamer;

connection::connection() { BOOST_ASSERT(gst_is_initialized()); }

connection::~connection() = default;

boost::future<rtc::session_description> connection::create_offer() {
  boost::promise<rtc::session_description> promise;
  auto result = promise.get_future();
  promise.set_value(rtc::session_description{});
  return result;
}

boost::future<void>
connection::set_local_description(const session_description &) {
  boost::promise<void> promise;
  auto result = promise.get_future();
  return result;
}

boost::future<void>
connection::set_remote_description(const session_description &) {
  boost::promise<void> promise;
  auto result = promise.get_future();
  return result;
}
void connection::add_track(track_ptr) {}
