#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string_view>
extern "C" {
#include <gst/gst.h>
}

// tutorial
// https://github.com/centricular/gstwebrtc-demos/blob/master/sendrecv/gst/webrtc-sendrecv.c

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

static void on_negotiation_needed(GstElement * /*element*/,
                                  gpointer /*user_data*/) {
  std::cout << "on_negotiation_needed" << std::endl;
}

static void on_ice_candidate(GstElement * /*webrtc*/, guint /*mlineindex*/,
                             gchar * /*candidate*/, gpointer /*user_data*/) {
  std::cout << "on_ice_candidate" << std::endl;
}

static void on_pad_added(GstElement * /*webrtc*/, GstPad * /*pad*/,
                         GstElement * /*pipe*/) {}

static void setup_pipeline() {
  GError *error{nullptr};
  GstElement *pipe1 = gst_parse_launch(
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

  GstElement *webrtc1 = gst_bin_get_by_name(GST_BIN(pipe1), "sendrecv");
  assert(webrtc1);

  /* This is the gstwebrtc entry point where we create the offer and so on. It
   * will be called when the pipeline goes to PLAYING. */
  g_signal_connect(webrtc1, "on-negotiation-needed",
                   G_CALLBACK(on_negotiation_needed), nullptr);
  /* We need to transmit this ICE candidate to the browser via the websockets
   * signalling server. Incoming ice candidates from the browser need to be
   * added by us too, see on_server_message() */
  g_signal_connect(webrtc1, "on-ice-candidate", G_CALLBACK(on_ice_candidate),
                   nullptr);
  /* Incoming streams will be exposed via this signal */
  g_signal_connect(webrtc1, "pad-added", G_CALLBACK(on_pad_added), pipe1);
  /* Lifetime is the same as the pipeline itself */
  gst_object_unref(webrtc1);

  g_print("Starting pipeline\n");
  GstStateChangeReturn ret =
      gst_element_set_state(GST_ELEMENT(pipe1), GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
    throw std::runtime_error("gst_element_set_state, GST_STATE_PLAYING");
}

int main(int argc, char *argv[]) {
  static_assert(__cplusplus >= 201703L);
  print_gstreamer_version();
  initialise_gstreamer(&argc, &argv);
  check_plugins();
  setup_pipeline();
  return 0;
}
