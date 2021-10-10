#define GST_USE_UNSTABLE_API
#include <boost/assert.hpp>
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <fmt/format.h>
#include <fubble/utils/logging/logger.hpp>
#include <gst/gst.h>
#include <gst/gstpromise.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>

namespace {
struct peer {
  GstElement *pipe{};
  GstElement *webrtc{};
  guint bus_watch_id{};
};
struct offering {
  peer peer_;
};
struct answering {
  peer peer_;
};
} // namespace
static void print_gerror(GError *error) { (void)error; }
static void check_plugins();
static void check_version();
static void init_offering();
static void init_answering();
static void init_peer(peer &peer_, const std::string &pattern);
static void on_negotiation_needed(GstElement *webrtc, gpointer user_data);
static void on_ice_candidate(GstElement * /*webrtc*/, guint mlineindex,
                             gchar *candidate, gpointer /*user_data*/);
static void on_offer_created(GstPromise *promise, gpointer webrtc);
static gboolean on_pipe_bus_message(GstBus *bus, GstMessage *message,
                                    gpointer data);
static void on_incoming_stream(GstElement *webrtc, GstPad *pad,
                               gpointer user_data);

static logging::logger logger{"main"};
static offering offering_;
static answering answering_;
static GMainLoop *loop;

int main(int argc, char *argv[]) {
  boost::log::add_common_attributes();
  boost::log::add_console_log(
      std::cout, boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "%TimeStamp% %Severity% %Message%");

  GError *error{};
  if (!gst_init_check(&argc, &argv, &error)) {
    print_gerror(error);
    return 1;
  }
  check_plugins();
  check_version();
  init_offering();
  init_answering();

  BOOST_LOG_SEV(logger, logging::severity::info) << "done, with init";

  loop = g_main_loop_new(nullptr, false);
  g_main_loop_run(loop);

  BOOST_LOG_SEV(logger, logging::severity::info) << "done";
  gst_deinit();

  return 0;
}
static void init_offering() { init_peer(offering_.peer_, "ball"); }
static void init_answering() { init_peer(answering_.peer_, "smpte"); }

static void init_peer(peer &peer_, const std::string &pattern) {
  bool is_offering = &peer_ == &offering_.peer_;
  std::string picture_description = is_offering ? "offering" : "answering";
  GError *error{};
  std::string launch_text = fmt::format(
      "webrtcbin name=sendrecv "
      "videotestsrc is-live=true pattern={} ! "
      "video/x-raw,width=500,height=500,framerate=60/1 ! "
      "videoconvert ! "
      "clockoverlay halignment=right valignment=top text=\"{}\" "
      "shaded-background=true font-desc=\" Sans 36 \" ! "
      "queue ! "
      "vp8enc deadline=1 ! "
      "rtpvp8pay ! "
      "queue ! "
      "application/x-rtp,media=video,encoding-name=VP8,payload=96 ! "
      "sendrecv. ",
      pattern, picture_description);
  GstElement *pipe = gst_parse_launch(launch_text.c_str(), &error);
  if (!pipe)
    throw std::runtime_error(fmt::format(
        "failed to parse launch. error message: '{}'", error->message));
  GstElement *webrtc = gst_bin_get_by_name(GST_BIN(pipe), "sendrecv");
  if (!webrtc)
    throw std::runtime_error("could not get webrtc element");
  peer_.pipe = pipe;
  peer_.webrtc = webrtc;

  gst_element_set_state(pipe, GST_STATE_READY);

  /* This is the gstwebrtc entry point where we create the offer.
   * It will be called when the pipeline goes to PLAYING. */
  g_signal_connect(webrtc, "on-negotiation-needed",
                   G_CALLBACK(on_negotiation_needed), &peer_);
  /* We will transmit this ICE candidate to the remote using some
   * signaling. Incoming ICE candidates from the remote need to be
   * added by us too. */
  g_signal_connect(webrtc, "on-ice-candidate", G_CALLBACK(on_ice_candidate),
                   &peer_);
  /* Incoming streams will be exposed via this signal */
  g_signal_connect(webrtc, "pad-added", G_CALLBACK(on_incoming_stream), &peer_);
  // g_signal_connect (webrtc, "on-data-channel", G_CALLBACK (on_data_channel),
  // NULL);

  auto bus = gst_pipeline_get_bus(GST_PIPELINE(pipe));
  peer_.bus_watch_id = gst_bus_add_watch(bus, on_pipe_bus_message, &peer_);
  gst_object_unref(bus);
#if 0
  gst_element_set_state(pipe, GST_STATE_READY);

  static GObject *send_channel;
  g_signal_emit_by_name(webrtc, "create-data-channel", "channel", NULL,
                        &send_channel);
  if (!send_channel) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not create data channel";
    return 1;
  }
#endif
  const GstStateChangeReturn state_change_result =
      gst_element_set_state(GST_ELEMENT(pipe), GST_STATE_PLAYING);
  if (state_change_result == GST_STATE_CHANGE_FAILURE)
    throw std::runtime_error("gst_element_set_state, GST_STATE_PLAYING");
}
static void throw_if_not_all_plugins_found(bool all_found) {
  if (all_found)
    return;
  throw std::runtime_error("could not find all needed plugins");
}
static void check_plugins() {
  static constexpr std::array needed = {
      "opus", "vpx",        "nice",         "webrtc",      "dtls",
      "srtp", "rtpmanager", "videotestsrc", "audiotestsrc"};
  GstRegistry *registry = gst_registry_get();
  bool all_found = true;
  for (const auto &check : needed) {
    GstPlugin *plugin = gst_registry_find_plugin(registry, check);
    if (!plugin) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "Required gstreamer plugin '" << check << "' not found";
      all_found = false;
      continue;
    }
    gst_object_unref(plugin);
  }
  throw_if_not_all_plugins_found(all_found);
}
static void check_version() {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("gst version: '{}'", gst_version_string());
  guint major, minor, patch, micro;
  gst_version(&major, &minor, &patch, &micro);
  BOOST_ASSERT(major == 1);
  if (minor < 16)
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "gstreamers minor version is less that '16({})'. there'll be no "
        "data-channel support. "
        "https://github.com/centricular/gstwebrtc-demos/issues/94",
        minor);
}

static void on_negotiation_needed(GstElement *webrtc, gpointer user_data) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_negotiation_needed";
  BOOST_ASSERT(webrtc);
  if (user_data == &answering_.peer_) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "answering device will wait for offer";
    return;
  }
  GstPromise *promise =
      gst_promise_new_with_change_func(on_offer_created, user_data, nullptr);
  g_signal_emit_by_name(webrtc, "create-offer", NULL, promise);
}

static void set_local_description(peer &peer_,
                                  GstWebRTCSessionDescription *sdp) {
  auto promise_set_local_description = gst_promise_new();
  g_signal_emit_by_name(peer_.webrtc, "set-local-description", sdp,
                        promise_set_local_description);
  gst_promise_interrupt(promise_set_local_description);
  gst_promise_unref(promise_set_local_description);
}

static void set_remote_description(peer &peer_,
                                   GstWebRTCSessionDescription *sdp) {
  auto promise_set_local_description = gst_promise_new();
  g_signal_emit_by_name(peer_.webrtc, "set-remote-description", sdp,
                        promise_set_local_description);
  gst_promise_interrupt(promise_set_local_description);
  gst_promise_unref(promise_set_local_description);
}

static void on_answer_created(GstPromise *promise, gpointer user_data) {
  (void)user_data;
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_answer_created";
  auto reply = gst_promise_get_reply(promise);
  GstWebRTCSessionDescription *answer;
  gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION,
                    &answer, nullptr);

  gchar *text = gst_sdp_message_as_text(answer->sdp);
  const std::string sdp_text = text;
  g_free(text);
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("answer sdp: '{}'", sdp_text);

  gst_promise_unref(promise);
  set_local_description(answering_.peer_, answer);
  set_remote_description(offering_.peer_, answer);
}

static void create_answer() {
  auto promise =
      gst_promise_new_with_change_func(on_answer_created, nullptr, nullptr);
  g_signal_emit_by_name(answering_.peer_.webrtc, "create-answer", nullptr,
                        promise);
}

static void send_offer(GstWebRTCSessionDescription *offer) {
  gchar *text = gst_sdp_message_as_text(offer->sdp);
  const std::string sdp_text = text;
  g_free(text);
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("got an offer sdp: '{}'", sdp_text);

  GstSDPMessage *sdp{};
  [[maybe_unused]] GstSDPResult result = gst_sdp_message_new(&sdp);
  BOOST_ASSERT(result == GstSDPResult::GST_SDP_OK);
  static_assert(sizeof(char) == sizeof(guint8));
  result = gst_sdp_message_parse_buffer(
      reinterpret_cast<const guint8 *>(sdp_text.data()), sdp_text.size(), sdp);
  BOOST_ASSERT(result == GstSDPResult::GST_SDP_OK);
  GstWebRTCSessionDescription *offer_sdp =
      gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_OFFER, sdp);
  BOOST_ASSERT(offer_sdp);
  if (true) // TODO set false, to test promise error
    set_remote_description(answering_.peer_, offer_sdp);
  // no deref?
  create_answer();
}

static void on_offer_created(GstPromise *promise, gpointer user_data) {
  (void)user_data;
  const GstStructure *reply = gst_promise_get_reply(promise);
  GstWebRTCSessionDescription *offer{};
  gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer,
                    nullptr);
  gst_promise_unref(promise);
  BOOST_ASSERT(offer);

  set_local_description(offering_.peer_, offer);

  BOOST_LOG_SEV(logger, logging::severity::info) << "got an offer";
  send_offer(offer);
  gst_webrtc_session_description_free(offer);
}

static void on_ice_candidate(GstElement *webrtc, guint mlineindex,
                             gchar *candidate, gpointer /*user_data*/) {
  const bool is_offering = webrtc == offering_.peer_.webrtc;
  const std::string kind = is_offering ? "offering" : "answering";
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "on_ice_candidate, kind:'{}', mlineindex:'{}', candidate:'{}'", kind,
      candidate, mlineindex);
  auto other_webrtc =
      is_offering ? answering_.peer_.webrtc : offering_.peer_.webrtc;
  BOOST_ASSERT(other_webrtc);
  g_signal_emit_by_name(other_webrtc, "add-ice-candidate", mlineindex,
                        candidate);
}

static void handle_video_stream(GstPad *pad, GstElement *pipe) {
  BOOST_ASSERT(pad);
  BOOST_ASSERT(pipe);
  BOOST_LOG_SEV(logger, logging::severity::info) << "handle_video_stream";
  auto queue = gst_element_factory_make("queue", nullptr);
  auto videoconvert = gst_element_factory_make("videoconvert", nullptr);
  auto clockoverlay = gst_element_factory_make("clockoverlay", nullptr);
  // ubuntu 18.04. autovideosink does not work. shows black screen.
  auto autovideosink = gst_element_factory_make("ximagesink", nullptr);
  BOOST_ASSERT(queue && videoconvert && clockoverlay && autovideosink);
  gst_bin_add_many(GST_BIN(pipe), queue, videoconvert, clockoverlay,
                   autovideosink, nullptr);
  gst_element_sync_state_with_parent(queue);
  gst_element_sync_state_with_parent(videoconvert);
  gst_element_sync_state_with_parent(clockoverlay);
  gst_element_sync_state_with_parent(autovideosink);
  gst_element_link_many(queue, videoconvert, clockoverlay, autovideosink,
                        nullptr);
  auto destination = queue;
  // config clockoverlay
  {
    // https://gstreamer.freedesktop.org/documentation/pango/clockoverlay.html
    g_object_set(clockoverlay, "shaded-background", 1, nullptr);
    g_object_set(clockoverlay, "font-desc", "Sans 36", nullptr);
    g_object_set(clockoverlay, "text", "local", nullptr);
    g_object_set(clockoverlay, "ypad", 36 + 25 + 36 / 2, nullptr);
    g_object_set(clockoverlay, "halignment", 2, nullptr);
  }
  { g_object_set(autovideosink, "sync", 0, nullptr); }
  GstPad *queue_pad = gst_element_get_static_pad(destination, "sink");
  [[maybe_unused]] auto result = gst_pad_link(pad, queue_pad);
  BOOST_ASSERT(result == GST_PAD_LINK_OK);
  BOOST_LOG_SEV(logger, logging::severity::info) << "there should be a video";
}

static void on_incoming_decodebin_stream(GstElement *decodebin, GstPad *pad,
                                         gpointer user_data) {
  (void)decodebin;
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "on_incoming_decodebin_stream, pad_name: '{}'", GST_PAD_NAME(pad));
  if (!gst_pad_has_current_caps(pad))
    throw std::runtime_error("!gst_pad_has_current_caps(pad)");
  GstCaps *caps = gst_pad_get_current_caps(pad);
  BOOST_ASSERT(caps);
  const gchar *name = gst_structure_get_name(gst_caps_get_structure(caps, 0));
  BOOST_ASSERT(name);
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("caps name:{}", name);
  peer &peer_ = *static_cast<peer *>(user_data);
  bool is_offering = &peer_ == &offering_.peer_;
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "on_incoming_decodebin_stream is_offering:{}", is_offering);
  handle_video_stream(pad, peer_.pipe);

  gst_caps_unref(caps);
}

static void on_incoming_stream(GstElement *webrtc, GstPad *pad,
                               gpointer user_data) {
  (void)webrtc;
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("on_incoming_stream, pad_name: '{}'", GST_PAD_NAME(pad));
  BOOST_ASSERT(GST_PAD_DIRECTION(pad) == GST_PAD_SRC);

  GstElement *decodebin = gst_element_factory_make("decodebin", nullptr);
  peer &peer_ = *static_cast<peer *>(user_data);
  g_signal_connect(decodebin, "pad-added",
                   G_CALLBACK(on_incoming_decodebin_stream), user_data);
  gst_bin_add(GST_BIN(peer_.pipe), decodebin);
  gst_element_sync_state_with_parent(decodebin);
  GstPad *sinkpad = gst_element_get_static_pad(decodebin, "sink");
  gst_pad_link(pad, sinkpad);
  gst_object_unref(sinkpad);
}

static gboolean on_pipe_bus_message(GstBus *bus, GstMessage *message,
                                    gpointer data) {
  (void)bus;
  (void)data;
  //  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
  //      "on_pipe_bus_message, got message: '{}'",
  //      GST_MESSAGE_TYPE_NAME(message));
  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_ERROR:
    GError *error;
    gchar *debug;
    gst_message_parse_error(message, &error, &debug);
    BOOST_LOG_SEV(logger, logging::severity::error)
        << fmt::format("error: '{}', debug: '{}'", error->message, debug);
    g_error_free(error);
    g_free(debug);
    break;
  default:;
    // BOOST_LOG_SEV(logger, logging::severity::info) << "unhandled message";
  }
  return true;
}
