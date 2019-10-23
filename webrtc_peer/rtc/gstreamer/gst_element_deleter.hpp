#ifndef RTC_GSTREAMER_GST_ELEMENT_DELETER_HPP
#define RTC_GSTREAMER_GST_ELEMENT_DELETER_HPP

#include <gst/gst.h>

namespace rtc::gstreamer {
struct gst_element_deleter {
  void operator()(GstElement *delete_me);
};
} // namespace rtc::gstreamer

#endif

