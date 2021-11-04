#ifndef UUID_5476BE8C_AEE9_4886_B354_F5AE636C6453
#define UUID_5476BE8C_AEE9_4886_B354_F5AE636C6453

#include <api/rtc_error.h>
#include <fubble/utils/exception.hpp>

namespace rtc {
namespace google {
struct google_rtc_error : utils::exception {};
using google_rtc_error_info =
    boost::error_info<struct google_rtc_error_tag, webrtc::RTCError>;
template <class T> T throw_on_error_or_value(webrtc::RTCErrorOr<T> &&check) {
  if (check.ok())
    return check.MoveValue();
  BOOST_THROW_EXCEPTION(google_rtc_error()
                        << google_rtc_error_info(check.MoveError()));
}
} // namespace google
} // namespace rtc

#endif
