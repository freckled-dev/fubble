#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string_view>
#include <thread>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
#include "connection.hpp"
#include "websocket/server.hpp"

// tutorial
// https://github.com/centricular/gstwebrtc-demos/blob/master/sendrecv/gst/webrtc-sendrecv.c

static GstElement *webrtc_element;

std::ostream &operator<<(std::ostream &out, const GError &error) {
  return out << "{domain:" << error.domain << ", code:" << error.code
             << ", description:" << error.message << "}";
}

static void free_and_throw_if_error(
    GError *error, const std::string_view additional_information) {
  if (error == nullptr) return;
  std::stringstream out;
  out << "an error occured, information: '" << additional_information
      << "', error:" << (*error) << std::endl;
  g_error_free(error);
  throw std::runtime_error(out.str());
}

static void initialise_gstreamer(int *argc, char **argv[]) {
  GError *error{nullptr};
  if (gst_init_check(argc, argv, &error)) return;
  free_and_throw_if_error(error, "could not initialise gstreamer");
}

static void print_gstreamer_version() {
  std::cout << "gstreamer version:" << gst_version_string() << std::endl;
}

static void throw_if_not_all_plugins_found(bool all_found) {
  if (all_found) return;
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
      std::cout << "Required gstreamer plugin '" << check << "' not found\n";
      all_found = false;
      continue;
    }
    gst_object_unref(plugin);
  }
  throw_if_not_all_plugins_found(all_found);
}

static void set_local_description(GstWebRTCSessionDescription *offer) {
  GstPromise *promise = gst_promise_new();
  g_signal_emit_by_name(webrtc_element, "set-local-description", offer,
                        promise);
  gst_promise_interrupt(promise);
  gst_promise_unref(promise);
}

static std::unique_ptr<connection> connection_web;
static std::string sdp;
struct ice_canditate {
  int mlineindex;
  std::string candidate;
};
static std::vector<ice_canditate> ice_canditates;

static void save_offer(GstWebRTCSessionDescription *offer) {
  gchar *text = gst_sdp_message_as_text(offer->sdp);
  std::string text_casted(text);
  sdp = std::move(text_casted);
  std::cout << "save_offer, sdp:" << sdp << std::endl;
  g_free(text);
}

static void on_offer_created(GstPromise *promise, gpointer /*user_data*/) {
  std::cout << "on_offer_created, thread_id:" << std::this_thread::get_id()
            << std::endl;
  assert(gst_promise_wait(promise) == GST_PROMISE_RESULT_REPLIED);
  const GstStructure *reply = gst_promise_get_reply(promise);
  GstWebRTCSessionDescription *offer{nullptr};
  if (!gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION,
                         &offer, nullptr))
    throw std::runtime_error("gst_structure_get(offer");
  gst_promise_unref(promise);
  set_local_description(offer);

  save_offer(offer);

  gst_webrtc_session_description_free(offer);
}

static void create_offer() {
  GstPromise *promise =
      gst_promise_new_with_change_func(on_offer_created, nullptr, nullptr);
  g_signal_emit_by_name(webrtc_element, "create-offer", NULL, promise);
}

static void on_negotiation_needed(GstElement * /*webrtc*/,
                                  gpointer /*user_data*/) {
  std::cout << "on_negotiation_needed, thread_id:" << std::this_thread::get_id()
            << std::endl;
  create_offer();
}

static void on_ice_candidate(GstElement * /*webrtc*/, guint mlineindex,
                             gchar *candidate, gpointer /*user_data*/) {
  const int mlineindex_casted = static_cast<int>(mlineindex);
  std::string canditate_casted = candidate;
  std::cout << "on_ice_candidate, mlineindex_casted:" << mlineindex_casted
            << ", canditate_casted:" << canditate_casted << std::endl;
  ice_canditates.push_back({mlineindex_casted, std::move(canditate_casted)});
}

static void on_pad_added(GstElement * /*webrtc*/, GstPad * /*pad*/,
                         GstElement * /*pipe*/) {}

static GstElement *setup_pipeline() {
  GError *error{nullptr};
  GstElement *main_pipe = gst_parse_launch(
      "webrtcbin name=sendrecv stun-server=stun://stun.l.google.com:19302 "
      "videotestsrc pattern=ball ! videoconvert ! queue ! "
      "vp8enc deadline=1 ! rtpvp8pay ! "
      "queue ! application/x-rtp,media=video,encoding-name=VP8,payload=96 ! "
      "sendrecv. "
      "audiotestsrc wave=red-noise ! audioconvert ! "
      "audioresample ! queue ! opusenc ! rtpopuspay ! "
      "queue ! application/x-rtp,media=audio,encoding-name=OPUS,payload=97 ! "
      "sendrecv. ",
      &error);
  free_and_throw_if_error(error, "gst_parse_launch");

  webrtc_element = gst_bin_get_by_name(GST_BIN(main_pipe), "sendrecv");
  assert(webrtc_element);

  /* This is the gstwebrtc entry point where we create the offer and so on. It
   * will be called when the pipeline goes to PLAYING. */
  g_signal_connect(webrtc_element, "on-negotiation-needed",
                   G_CALLBACK(on_negotiation_needed), nullptr);
  /* We need to transmit this ICE candidate to the browser via the websockets
   * signalling server. Incoming ice candidates from the browser need to be
   * added by us too, see on_server_message() */
  g_signal_connect(webrtc_element, "on-ice-candidate",
                   G_CALLBACK(on_ice_candidate), nullptr);
  /* Incoming streams will be exposed via this signal */
  g_signal_connect(webrtc_element, "pad-added", G_CALLBACK(on_pad_added),
                   main_pipe);
  /* Lifetime is the same as the pipeline itself */
  gst_object_unref(webrtc_element);

  g_print("Starting pipeline\n");
  const GstStateChangeReturn state_change_result =
      gst_element_set_state(GST_ELEMENT(main_pipe), GST_STATE_PLAYING);
  if (state_change_result == GST_STATE_CHANGE_FAILURE)
    throw std::runtime_error("gst_element_set_state, GST_STATE_PLAYING");
  return main_pipe;
}

int main(int argc, char *argv[]) {
  boost::asio::io_context context;
  constexpr port port_{8765};
  server::async_accept_callback callback =
      [](std::unique_ptr<connection> &&connection_) {
        std::cout << "accepted a new websocket connection" << std::endl;
        connection_web = std::move(connection_);
        connection_web->read_async([](std::string) {});
      };
  websocket::server server_{context, port_, callback};
  server_.start();
  std::thread server_thread([&context] { context.run(); });

  std::cout << "main thread_id:" << std::this_thread::get_id() << std::endl;
  static_assert(__cplusplus >= 201703L);
  print_gstreamer_version();
  initialise_gstreamer(&argc, &argv);
  check_plugins();
  GstElement *pipeline = setup_pipeline();
  GstBus *bus = gst_element_get_bus(pipeline);
  GstMessageType filter =
      static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  //  GstMessage *message =
  gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, filter);
  std::cout << "shutting down" << std::endl;
  server_thread.join();

  return 0;
}
