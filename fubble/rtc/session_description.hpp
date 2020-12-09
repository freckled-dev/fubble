#ifndef RTC_SESSION_DESCRIPTION_HPP
#define RTC_SESSION_DESCRIPTION_HPP

#include <string>

namespace rtc {
struct session_description {
  enum class type { offer, answer };
  type type_;
  std::string sdp;
};
} // namespace rtc

#endif
