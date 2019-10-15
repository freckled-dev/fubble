extern "C" {
#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}
// #include "signalling/client.hpp"

__attribute__((no_sanitize_address)) bool init() {
  GError *error{};
  if (!gst_init_check(nullptr, nullptr, &error))
    return false;
  return true;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  init();

#if 0
  GstElement *pipe = gst_parse_launch(
      "v4l2src ! queue ! vp8enc ! rtpvp8pay ! "
      "application/x-rtp,media=video,encoding-name=VP8,payload=96 !"
      " webrtcbin name=sendrecv",
      NULL);

  GstElement *webrtc = gst_bin_get_by_name(GST_BIN(pipe), "sendrecv");
  g_assert(webrtc != NULL);

  gst_object_unref(webrtc);
  gst_object_unref(pipe);
#endif

#if 1
  gst_deinit();
#endif

  return 0;
}
