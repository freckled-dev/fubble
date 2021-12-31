#include "connection.hpp"
#include "fubble/rtc/track.hpp"
#include "video_track.hpp"
#include <fmt/format.h>
extern "C" {
#include <gst/gstpromise.h>
#include <gst/sdp/sdp.h>
}

using namespace rtc::gstreamer;

static GstElement *create_element(const std::string &name) {
  auto result = gst_element_factory_make(name.c_str(), nullptr);
  BOOST_ASSERT(result);
  return result;
}
static void set_gst_state(GstElement *pipeline, const GstState state) {
  const GstStateChangeReturn state_change_result =
      gst_element_set_state(pipeline, state);
  BOOST_ASSERT(state_change_result != GST_STATE_CHANGE_ASYNC);
  BOOST_ASSERT(state_change_result == GST_STATE_CHANGE_SUCCESS);
  if (state_change_result == GST_STATE_CHANGE_FAILURE)
    throw std::runtime_error("gst_element_set_state, GST_STATE_PLAYING");
}

connection::connection(boost::executor &executor) : executor(executor) {
  BOOST_ASSERT(gst_is_initialized());
  pipeline.reset(gst_pipeline_new(nullptr));
  BOOST_ASSERT(pipeline);
  webrtc = create_element("webrtcbin");
  [[maybe_unused]] auto added = gst_bin_add(pipeline_as_bin(), webrtc);
  BOOST_ASSERT(added);
  connect_signals();
  auto bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline.get()));
  bus_watch_id = gst_bus_add_watch(bus, on_pipe_bus_message, this);
  gst_object_unref(bus);
}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "~connection()";
  try {
    close();
  } catch (std::runtime_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not stop pipeline";
  }
}

gboolean connection::on_pipe_bus_message(GstBus *, GstMessage *message,
                                         gpointer data) {
  auto self = static_cast<connection *>(data);
  BOOST_ASSERT(self);
  BOOST_LOG_SEV(self->logger, logging::severity::info) << fmt::format(
      "on_pipe_bus_message, got message: '{}'", GST_MESSAGE_TYPE_NAME(message));
  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_ERROR:
    GError *error;
    gchar *debug;
    gst_message_parse_error(message, &error, &debug);
    BOOST_LOG_SEV(self->logger, logging::severity::error)
        << fmt::format("error: '{}', debug: '{}'", error->message, debug);
    g_error_free(error);
    g_free(debug);
    break;
  case GST_MESSAGE_EOS:
    BOOST_LOG_SEV(self->logger, logging::severity::info) << "GST_MESSAGE_EOS";
    self->on_closed();
    break;
  default:
    // BOOST_LOG_SEV(logger, logging::severity::info) << "unhandled message";
    break;
  }
  return true;
}

boost::future<void> connection::run() {
  set_gst_state(pipeline.get(), GST_STATE_READY);
#if 1
  set_gst_state(pipeline.get(), GST_STATE_PLAYING);
#endif
  return boost::make_ready_future();
}

void connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "close";
  disconnect_signals();
  set_gst_state(pipeline.get(), GST_STATE_NULL);
}

boost::future<rtc::session_description>
connection::create_offer(const offer_options &) {
  auto promise = new boost::promise<rtc::session_description>();
  auto result = promise->get_future();
  GstPromise *gst_promise =
      gst_promise_new_with_change_func(on_offer_created, promise, nullptr);
  g_signal_emit_by_name(webrtc, "create-offer", nullptr, gst_promise);
  return result;
}

boost::future<rtc::session_description> connection::create_answer() {
  auto promise = new boost::promise<rtc::session_description>();
  auto result = promise->get_future();
  GstPromise *gst_promise =
      gst_promise_new_with_change_func(on_answer_created, promise, nullptr);
  g_signal_emit_by_name(webrtc, "create-answer", nullptr, gst_promise);
  return result;
}

boost::future<void>
connection::set_local_description(const session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "set_local_description, sdp:" << description.sdp;
  return set_description("set-local-description", description);
}

boost::future<void>
connection::set_remote_description(const session_description &description) {
  return set_description("set-remote-description", description);
}

boost::future<void>
connection::set_description(const std::string &kind,
                            const session_description &description) {
  auto promise = std::make_unique<boost::promise<void>>();
  auto result = promise->get_future();
  try {
    auto sdp = cast_session_description_to_gst(description);
    auto promise_gst = gst_promise_new_with_change_func(
        on_description_set, promise.release(), nullptr);
    g_signal_emit_by_name(webrtc, kind.c_str(), sdp, promise_gst);
    gst_webrtc_session_description_free(sdp);
  } catch (const invalid_session_description_sdp &error) {
    promise->set_exception(error);
  }
  return result;
}

void connection::add_track(track_ptr track_) {
  auto track_casted = dynamic_cast<video_track *>(track_.get());
  BOOST_ASSERT(track_casted);
  track_casted->link_to_webrtc(get_natives());
}

rtc::data_channel_ptr connection::create_data_channel() {
  BOOST_ASSERT(false && "not implemented");
  return nullptr;
}

void connection::remove_track(track_ptr) {
  BOOST_ASSERT(false && "not implemented");
}

void connection::get_stats(const stats_callback &) {
  BOOST_ASSERT(false && "not implemented");
}

void connection::add_ice_candidate(const ice_candidate &candidate) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "adding ice_candidate:" << candidate;
  auto mlineindex_casted = static_cast<gint>(candidate.mlineindex);
  g_signal_emit_by_name(webrtc, "add-ice-candidate", mlineindex_casted,
                        candidate.sdp.c_str());
}

connection::state connection::get_state() {
  int cast{};
  g_object_get(webrtc, "connection-state", &cast, nullptr);
  switch (cast) {
  case 0:
    return state::new_;
  case 1:
    return state::connecting;
  case 2:
    return state::connected;
  case 3:
    return state::disconnected;
  case 4:
    return state::failed;
  case 5:
    return state::closed;
  }
  BOOST_ASSERT(false);
  return state::closed;
}

connection::signaling_state connection::get_signaling_state() {
  int cast{};
  g_object_get(webrtc, "signaling-state", &cast, nullptr);
  switch (cast) {
  case 0:
    return signaling_state::stable;
  case 1:
    return signaling_state::closed;
  case 2:
    return signaling_state::have_local_offer;
  case 3:
    return signaling_state::have_remote_offer;
  case 4:
    return signaling_state::have_local_pranswer;
  case 5:
    return signaling_state::have_remote_pranswer;
  }
  BOOST_ASSERT(false);
  return signaling_state::closed;
}

connection::ice_gathering_state connection::get_ice_gathering_state() {
  int cast{};
  g_object_get(webrtc, "ice-gathering-state", &cast, nullptr);
  switch (cast) {
  case 0:
    return ice_gathering_state::new_;
  case 1:
    return ice_gathering_state::gathering;
  case 2:
    return ice_gathering_state::complete;
  }
  BOOST_ASSERT(false);
  return ice_gathering_state::new_;
}

connection::ice_connection_state connection::get_ice_connection_state() {
  int cast{};
  g_object_get(webrtc, "ice-connection-state", &cast, nullptr);
  switch (cast) {
  case 0:
    return ice_connection_state::new_;
  case 1:
    return ice_connection_state::checking;
  case 2:
    return ice_connection_state::connected;
  case 3:
    return ice_connection_state::completed;
  case 4:
    return ice_connection_state::failed;
  case 5:
    return ice_connection_state::disconnected;
  case 6:
    return ice_connection_state::closed;
  }
  BOOST_ASSERT(false);
  return ice_connection_state::new_;
}

connection *connection::cast_user_data_to_connection(gpointer user_data) {
  BOOST_ASSERT(user_data);
  return static_cast<connection *>(user_data);
}

void connection::on_gst_negotiation_needed(GstElement *webrtc,
                                           gpointer user_data) {
  BOOST_ASSERT(webrtc);
  BOOST_ASSERT(user_data);
  auto self = cast_user_data_to_connection(user_data);
  BOOST_LOG_SEV(self->logger, logging::severity::info)
      << "on_gst_negotiation_needed";
  self->executor.submit([self, webrtc] {
    (void)webrtc;
    BOOST_ASSERT(self->webrtc == webrtc);
    BOOST_LOG_SEV(self->logger, logging::severity::info)
        << "on_negotiation_needed";
    self->on_negotiation_needed();
  });
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
  result.type_ = session_description::type::offer;
  promise->set_value(result);
  delete promise;
}

void connection::on_answer_created(GstPromise *gst_promise,
                                   gpointer user_data) {
  BOOST_ASSERT(gst_promise);
  BOOST_ASSERT(user_data);
  auto promise = static_cast<boost::promise<session_description> *>(user_data);
  const GstStructure *reply = gst_promise_get_reply(gst_promise);
  GstWebRTCSessionDescription *answer{};
  gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION,
                    &answer, nullptr);
  gst_promise_unref(gst_promise);
  gchar *text = gst_sdp_message_as_text(answer->sdp);
  const std::string sdp_text = text;
  g_free(text);
  gst_webrtc_session_description_free(answer);
  session_description result;
  result.sdp = sdp_text;
  result.type_ = session_description::type::answer;
  promise->set_value(result);
  delete promise;
}

void connection::on_description_set(GstPromise *promise_gst,
                                    gpointer user_data) {
  rtc::logger logger{"connection::on_description_set"};
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_description_set";
  BOOST_ASSERT(promise_gst);
  BOOST_ASSERT(user_data);
  std::unique_ptr<boost::promise<void>> promise{
      static_cast<boost::promise<void> *>(user_data)};
  [[maybe_unused]] const GstStructure *reply =
      gst_promise_get_reply(promise_gst);
  BOOST_ASSERT(!reply);
  gst_promise_unref(promise_gst);
  promise->set_value();
}

void connection::on_gst_ice_candidate(GstElement *, guint mlineindex,
                                      gchar *candidate, gpointer user_data) {
  auto self = cast_user_data_to_connection(user_data);
  const ice_candidate result{static_cast<int>(mlineindex), "", candidate};
  BOOST_LOG_SEV(self->logger, logging::severity::info)
      << "on_gst_ice_candidate, candidate:" << result;
  BOOST_ASSERT(user_data);
  self->executor.submit([self, result] {
    BOOST_LOG_SEV(self->logger, logging::severity::info)
        << "on_gst_ice_candidate submit, candidate:" << result;
    self->on_ice_candidate(result);
  });
}

void connection::connect_signals() {
  BOOST_ASSERT(signal_connections.empty());
  connect_signal("on-negotiation-needed",
                 G_CALLBACK(on_gst_negotiation_needed));
  connect_signal("on-ice-candidate", G_CALLBACK(on_gst_ice_candidate));
  // g_signal_connect(webrtc, "pad-added", G_CALLBACK(on_incoming_stream),
  // this);
}

void connection::connect_signal(const std::string &name, GCallback function) {
  const auto id = g_signal_connect(webrtc, name.c_str(), function, this);
  BOOST_ASSERT(id > 0);
  signal_connections.push_back(id);
}

void connection::disconnect_signals() {
  for (const auto id : signal_connections)
    g_signal_handler_disconnect(webrtc, id);
  signal_connections.clear();
}

GstBin *connection::pipeline_as_bin() const {
  BOOST_ASSERT(pipeline);
  return GST_BIN(pipeline.get());
}

GstWebRTCSessionDescription *connection::cast_session_description_to_gst(
    const session_description &description) {
  GstSDPMessage *sdp{};
  GstSDPResult result = gst_sdp_message_new(&sdp);
  BOOST_ASSERT(result == GstSDPResult::GST_SDP_OK);
  static_assert(sizeof(char) == sizeof(guint8));
  result = gst_sdp_message_parse_buffer(
      reinterpret_cast<const guint8 *>(description.sdp.data()),
      description.sdp.size(), sdp);
  if (result != GstSDPResult::GST_SDP_OK) {
    gst_sdp_message_free(sdp);
    throw invalid_session_description_sdp(description.sdp);
  }
  GstWebRTCSDPType type_casted = [&] {
    switch (description.type_) {
    case session_description::type::answer:
      return GST_WEBRTC_SDP_TYPE_ANSWER;
    case session_description::type::offer:
      return GST_WEBRTC_SDP_TYPE_OFFER;
    }
    BOOST_ASSERT(false);
    return GST_WEBRTC_SDP_TYPE_OFFER;
  }();
  GstWebRTCSessionDescription *sdp_casted =
      gst_webrtc_session_description_new(type_casted, sdp);
  BOOST_ASSERT(sdp_casted);
  return sdp_casted;
}

connection::natives connection::get_natives() {
  return natives{pipeline.get(), webrtc};
}
