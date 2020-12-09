#include "video_track.hpp"
#include <fmt/format.h>

using namespace rtc::gstreamer;

video_track::parse_error::parse_error(GError *error)
    : std::runtime_error(error->message) {}

video_track::video_track() { initialise_gst(); }

video_track::~video_track() {
  if (!unref_bin)
    return;
  gst_object_unref(bin);
}

void video_track::initialise_gst() {
  const char *description =
      "videotestsrc is-live=true pattern=ball ! "
      "video/x-raw,width=500,height=500,framerate=60/1 ! "
      "videoconvert ! "
      // "clockoverlay halignment=right valignment=top text=\"encoding\" "
      // "shaded-background=true font-desc=\" Sans 36 \" ! "
      // "queue ! "
      "vp8enc deadline=1 ! "
      "rtpvp8pay ! "
      "application/x-rtp,media=video,encoding-name=VP8,payload=96 ! "
      "queue ";
  GError *error{};
  bin = gst_parse_bin_from_description(description, true, &error);
  if (error) {
    parse_error error_casted{error};
    g_error_free(error);
    throw error_casted;
  }
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "parsed bin description, source pad count: '{}', sink pad count: '{}'",
      bin->numsrcpads, bin->numsinkpads);
}

void video_track::link_to_webrtc(const connection::natives &natives) {
  gst_bin_add(GST_BIN(natives.pipeline), bin);
  gst_element_sync_state_with_parent(bin);
  gst_element_link(bin, natives.webrtc);
  // it now belongs to the webrtc elemnt. dont delete it no more!
  unref_bin = false;
}
