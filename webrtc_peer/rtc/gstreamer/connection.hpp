#include "logging/logger.hpp"
#include "rtc/connection.hpp"
extern "C" {
#include <gst/gst.h>
}

namespace rtc::gstreamer {
struct gst_element_deleter {
  void operator()(GstElement *delete_me);
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

private:
  static void on_negotiation_needed(GstElement *webrtc, gpointer user_data);
  static void on_offer_created(GstPromise *promise, gpointer user_data);
  static connection *cast_user_data_to_connection(gpointer user_data);
  GstBin *pipeline_as_bin() const;
  void connect_signals();

  logging::logger logger;
  std::unique_ptr<GstElement, gst_element_deleter> pipeline;
  GstElement *webrtc;
};
} // namespace rtc::gstreamer
