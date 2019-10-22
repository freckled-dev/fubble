#include "logging/logger.hpp"
#include "rtc/connection.hpp"
extern "C" {
#include <gst/gst.h>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

namespace rtc::gstreamer {
struct gst_element_deleter {
  void operator()(GstElement *delete_me);
};

struct invalid_session_description_sdp : std::runtime_error {
  invalid_session_description_sdp(const std::string &sdp)
      : std::runtime_error(sdp) {}
};

class connection : public rtc::connection {
public:
  connection();
  ~connection() override;
  boost::future<session_description> create_offer() override;
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;
  void add_track(track_ptr) override;
  enum struct state {
    new_,
    connecting,
    connected,
    disconnected,
    failed,
    closed
  };
  state get_state();
  enum struct signalling_state {
    stable,
    closed,
    have_local_offer,
    have_remote_offer,
    have_local_pranswer,
    have_remote_pranswer
  };
  signalling_state get_signalling_state();

private:
  static void on_negotiation_needed(GstElement *webrtc, gpointer user_data);
  static void on_offer_created(GstPromise *promise, gpointer user_data);
  static void on_description_set(GstPromise *promise, gpointer user_data);
  static connection *cast_user_data_to_connection(gpointer user_data);
  static GstWebRTCSessionDescription *
  cast_session_description_to_gst(const session_description &description);
  GstBin *pipeline_as_bin() const;
  void connect_signals();

  logging::logger logger;
  std::unique_ptr<GstElement, gst_element_deleter> pipeline;
  GstElement *webrtc;
};
} // namespace rtc::gstreamer
