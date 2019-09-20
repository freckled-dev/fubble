#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <array>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
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

static void send_string(connection &connection_,
                        const std::string_view to_send) {
  const std::vector<char> message(to_send.cbegin(), to_send.cend());
  connection_.write(message);
}

static void send_itc_canditate(connection &connection_,
                               const ice_canditate &candidate) {
  const std::string json =
      "{\"candidate\":{"
      "  \"candidate\":\"" +
      candidate.candidate +
      "\","
      //      "\"sdpMid\":\"video\","
      "\"sdpMLineIndex\":" +
      std::to_string(candidate.mlineindex) + "}}";
  send_string(connection_, json);
}

static void send_last_itc_canditate(connection &connection_) {
  const std::string json = "{\"candidate\":null}";
  send_string(connection_, json);
}

static void send_itc_canditates(connection &connection_) {
  for (const auto &canditate : ice_canditates)
    send_itc_canditate(connection_, canditate);
  //  send_last_itc_canditate(connection_);
}

static void send_sdp(connection &connection_) {
  std::string json =
      "{\"sdp\":{"
      "  \"type\":\"offer\","
      "  \"sdp\":\"" +
      sdp + "\"}}";
  boost::algorithm::replace_all(json, "\r", "\\r");
  boost::algorithm::replace_all(json, "\n", "\\n");
  send_string(connection_, json);
}

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
  //  std::cout << "on_ice_candidate, mlineindex_casted:" << mlineindex_casted
  //            << ", canditate_casted:" << canditate_casted << std::endl;
  ice_canditates.push_back({mlineindex_casted, std::move(canditate_casted)});
}

static void handle_media_stream(GstPad *pad, GstElement *pipe,
                                const char *convert_name,
                                const char *sink_name) {
  std::cout << "Trying to handle stream with " << convert_name << " ! "
            << sink_name << std::endl;

  GstElement *q = gst_element_factory_make("queue", nullptr);
  assert(q != nullptr);
  GstElement *conv = gst_element_factory_make(convert_name, nullptr);
  assert(conv != nullptr);
  GstElement *sink = gst_element_factory_make(sink_name, nullptr);
  assert(sink != nullptr);

  if (g_strcmp0(convert_name, "audioconvert") == 0) {
    /* Might also need to resample, so add it just in case.
     * Will be a no-op if it's not required. */
    GstElement *resample = gst_element_factory_make("audioresample", nullptr);
    assert(resample != nullptr);
    gst_bin_add_many(GST_BIN(pipe), q, conv, resample, sink, nullptr);
    gst_element_sync_state_with_parent(q);
    gst_element_sync_state_with_parent(conv);
    gst_element_sync_state_with_parent(resample);
    gst_element_sync_state_with_parent(sink);
    gst_element_link_many(q, conv, resample, sink, nullptr);
  } else {
    gst_bin_add_many(GST_BIN(pipe), q, conv, sink, nullptr);
    gst_element_sync_state_with_parent(q);
    gst_element_sync_state_with_parent(conv);
    gst_element_sync_state_with_parent(sink);
    gst_element_link_many(q, conv, sink, NULL);
  }

  GstPad *qpad = gst_element_get_static_pad(q, "sink");

  GstPadLinkReturn ret = gst_pad_link(pad, qpad);
  assert(ret == GST_PAD_LINK_OK);
}

static void on_incoming_decodebin_stream(GstElement * /*decodebin*/,
                                         GstPad *pad, GstElement *pipe) {
  std::cout << "on_incoming_decodebin_stream" << std::endl;
  if (!gst_pad_has_current_caps(pad)) {
    std::cout << "Pad has no caps, can't do anything, ignoring:"
              << GST_PAD_NAME(pad) << std::endl;
    return;
  }

  GstCaps *caps = gst_pad_get_current_caps(pad);
  const gchar *name = gst_structure_get_name(gst_caps_get_structure(caps, 0));

  if (g_str_has_prefix(name, "video")) {
    handle_media_stream(pad, pipe, "videoconvert", "autovideosink");
  } else if (g_str_has_prefix(name, "audio")) {
    handle_media_stream(pad, pipe, "audioconvert", "autoaudiosink");
  } else {
    std::cout << "Unknown pad, ignoring:" << GST_PAD_NAME(pad) << std::endl;
  }
}

static void on_pad_added(GstElement * /*decodebin*/, GstPad *pad,
                         GstElement *pipe) {
  std::cout << "on_pad_added" << std::endl;

  if (GST_PAD_DIRECTION(pad) != GST_PAD_SRC) return;

  GstElement *decodebin = gst_element_factory_make("decodebin", nullptr);
  g_signal_connect(decodebin, "pad-added",
                   G_CALLBACK(on_incoming_decodebin_stream), pipe);
  gst_bin_add(GST_BIN(pipe), decodebin);
  gst_element_sync_state_with_parent(decodebin);
  gst_element_link(webrtc_element, decodebin);
}

static GstElement *setup_pipeline() {
  GError *error{nullptr};
  GstElement *main_pipe = gst_parse_launch(
      "webrtcbin name=sendrecv "
  //      "stun-server=stun://stun.l.google.com:19302 "
#if 1
      "videotestsrc ! "
      "videoconvert ! queue ! "
      "vp8enc deadline=1 ! rtpvp8pay ! "
      "queue ! application/x-rtp,media=video,encoding-name=VP8,payload=96 ! "
      "fakesink "
#endif
#if 0
      "videotestsrc pattern=ball ! "
      "videoconvert ! "
      "vp8enc deadline=1 ! rtpvp8pay ! "
      "application/x-rtp,media=video,encoding-name=VP8,payload=98 ! "
      "sendrecv. "
#endif
#if 0
      "audiotestsrc wave=red-noise ! audioconvert ! "
      "audioresample ! queue ! opusenc ! rtpopuspay ! "
      "queue ! application/x-rtp,media=audio,encoding-name=OPUS,payload=97 ! "
      "sendrecv. "
#endif
      ,
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

static void handle_sdp(const boost::property_tree::ptree &json) {
  int return_value;
  GstSDPMessage *sdp;
  GstWebRTCSessionDescription *answer;
  return_value = gst_sdp_message_new(&sdp);
  assert(return_value == GST_SDP_OK);

  std::string sdp_value = json.get<std::string>("sdp");
  const guint8 *sdp_data = reinterpret_cast<const guint8 *>(sdp_value.data());
  const guint sdp_data_size = static_cast<guint>(sdp_value.length());
  return_value = gst_sdp_message_parse_buffer(sdp_data, sdp_data_size, sdp);
  assert(return_value == GST_SDP_OK);

  answer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
  assert(answer != nullptr);

  /* Set remote description on our pipeline */
  {
    GstPromise *promise = gst_promise_new();
    g_signal_emit_by_name(webrtc_element, "set-remote-description", answer,
                          promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);
  }
}

static void handle_ice_candidate(const boost::property_tree::ptree &json) {
  const boost::optional<std::string> candidate =
      json.get_optional<std::string>("candidate");
  if (!candidate) return;
  const gint sdpmlineindex = json.get<gint>("sdpMLineIndex");
  g_signal_emit_by_name(webrtc_element, "add-ice-candidate", sdpmlineindex,
                        candidate->c_str());
}

static void handle_json_message(std::string message) {
  boost::property_tree::ptree properties;
  std::stringstream stream;
  stream << message;
  boost::property_tree::read_json(stream, properties);
  const auto sdp = properties.get_child_optional("sdp");
  if (sdp) {
    handle_sdp(*sdp);
    return;
  }
  const auto candidate = properties.get_child_optional("candidate");
  if (candidate) {
    handle_ice_candidate(*candidate);
  }
}

int main(int argc, char *argv[]) {
  boost::asio::io_context context;
  constexpr port port_{8765};
  server::async_accept_callback callback =
      [](std::unique_ptr<connection> &&connection_) {
        std::cout << "accepted a new websocket connection" << std::endl;
        connection_web = std::move(connection_);
        connection_web->read_async([](std::string message) {
          handle_json_message(std::move(message));
        });
        send_sdp(*connection_web);
        send_itc_canditates(*connection_web);
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
