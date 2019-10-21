#include "connection.hpp"
extern "C" {
#include <gst/gstpromise.h>
#include <gst/sdp/sdp.h>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

using namespace rtc::gstreamer;

void gst_element_deleter::operator()(GstElement *delete_me) {
  gst_object_unref(delete_me);
}

static GstElement *create_element(const std::string &name) {
  auto result = gst_element_factory_make(name.c_str(), nullptr);
  BOOST_ASSERT(result);
  return result;
}

connection::connection() {
  BOOST_ASSERT(gst_is_initialized());
  pipeline.reset(gst_pipeline_new(nullptr));
  BOOST_ASSERT(pipeline);
  webrtc = create_element("webrtcbin");
  [[maybe_unused]] auto added = gst_bin_add(pipeline_as_bin(), webrtc);
  BOOST_ASSERT(added);
  connect_signals();
}

connection::~connection() = default;

boost::future<rtc::session_description> connection::create_offer() {
  auto promise = new boost::promise<rtc::session_description>();
  auto result = promise->get_future();
  GstPromise *gst_promise =
      gst_promise_new_with_change_func(on_offer_created, promise, nullptr);
  g_signal_emit_by_name(webrtc, "create-offer", nullptr, gst_promise);
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

connection *connection::cast_user_data_to_connection(gpointer user_data) {
  BOOST_ASSERT(user_data);
  return static_cast<connection *>(user_data);
}

void connection::on_negotiation_needed(GstElement *webrtc, gpointer user_data) {
  BOOST_ASSERT(webrtc);
  auto self = cast_user_data_to_connection(user_data);
  BOOST_ASSERT(self->webrtc == webrtc);
  BOOST_LOG_SEV(self->logger, logging::severity::info)
      << "on_negotiation_needed";
}

void connection::on_offer_created(GstPromise *gst_promise, gpointer user_data) {
  BOOST_ASSERT(gst_promise);
  BOOST_ASSERT(user_data);
  auto promise = static_cast<boost::promise<session_description> *>(user_data);
  const GstStructure *reply = gst_promise_get_reply(gst_promise);
  GstWebRTCSessionDescription *offer{};
  gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer,
                    nullptr);
  gst_promise_unref(gst_promise);
  gchar *text = gst_sdp_message_as_text(offer->sdp);
  const std::string sdp_text = text;
  g_free(text);
  gst_webrtc_session_description_free(offer);
  session_description result;
  result.sdp = sdp_text;
  promise->set_value(result);
  delete promise;
}

void connection::connect_signals() {
  g_signal_connect(webrtc, "on-negotiation-needed",
                   G_CALLBACK(on_negotiation_needed), this);
  // g_signal_connect(webrtc, "on-ice-candidate", G_CALLBACK(on_ice_candidate),
  // this); g_signal_connect(webrtc, "pad-added",
  // G_CALLBACK(on_incoming_stream), this);
}

GstBin *connection::pipeline_as_bin() const {
  BOOST_ASSERT(pipeline);
  return GST_BIN(pipeline.get());
}
