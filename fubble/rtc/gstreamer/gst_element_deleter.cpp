#include "gst_element_deleter.hpp"

using namespace rtc::gstreamer;

void gst_element_deleter::operator()(GstElement *delete_me) {
  gst_object_unref(delete_me);
}
