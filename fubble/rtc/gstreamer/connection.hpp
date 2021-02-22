#ifndef RTC_GSTREAMER_CONNECTION_HPP
#define RTC_GSTREAMER_CONNECTION_HPP

#include <fubble/rtc/connection.hpp>
#include <fubble/rtc/gstreamer/gst_element_deleter.hpp>
#include <fubble/rtc/logger.hpp>
// TODO move, implementation details to impl file
extern "C" {
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

namespace rtc::gstreamer {
struct invalid_session_description_sdp : std::runtime_error {
  invalid_session_description_sdp(const std::string &sdp)
      : std::runtime_error(sdp) {}
};

class connection : public rtc::connection {
public:
  connection(boost::executor &executor);
  ~connection() override;
  boost::future<void> run();
  void close() override;
  void add_track(track_ptr) override;
  void add_ice_candidate(const ice_candidate &candidate) override;
  boost::future<session_description>
  create_offer(const offer_options &options) override;
  boost::future<session_description> create_answer() override;
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;

  void remove_track(track_ptr) override;
  rtc::data_channel_ptr create_data_channel() override;
  void get_stats(const stats_callback &callback) override;

  enum struct state {
    new_,
    connecting,
    connected,
    disconnected,
    failed,
    closed
  };
  state get_state();
  enum struct signaling_state {
    stable,
    closed,
    have_local_offer,
    have_remote_offer,
    have_local_pranswer,
    have_remote_pranswer
  };
  signaling_state get_signaling_state();
  enum struct ice_gathering_state { new_, gathering, complete };
  ice_gathering_state get_ice_gathering_state();
  enum struct ice_connection_state {
    new_,
    checking,
    connected,
    completed,
    failed,
    disconnected,
    closed
  };
  ice_connection_state get_ice_connection_state();
  struct natives {
    GstElement *pipeline;
    GstElement *webrtc;
  };
  natives get_natives();

private:
  boost::future<void> set_description(const std::string &kind,
                                      const session_description &description);
  static void on_gst_negotiation_needed(GstElement *webrtc, gpointer user_data);
  static void on_offer_created(GstPromise *promise, gpointer user_data);
  static void on_answer_created(GstPromise *promise, gpointer user_data);
  static void on_description_set(GstPromise *promise, gpointer user_data);
  static void on_gst_ice_candidate(GstElement *webrtc, guint mlineindex,
                                   gchar *candidate, gpointer user_data);
  static gboolean on_pipe_bus_message(GstBus *bus, GstMessage *message,
                                      gpointer data);
  static connection *cast_user_data_to_connection(gpointer user_data);
  static GstWebRTCSessionDescription *
  cast_session_description_to_gst(const session_description &description);
  GstBin *pipeline_as_bin() const;
  void connect_signals();
  void connect_signal(const std::string &name, GCallback function);
  void disconnect_signals();

  rtc::logger logger{"connection"};
  boost::executor &executor;
  std::unique_ptr<GstElement, gst_element_deleter> pipeline;
  GstElement *webrtc;
  guint bus_watch_id{};
  std::vector<gulong> signal_connections;
};
} // namespace rtc::gstreamer

#endif
