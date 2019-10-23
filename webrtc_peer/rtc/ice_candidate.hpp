#ifndef RTC_ICE_CANDIDATE_HPP
#define RTC_ICE_CANDIDATE_HPP

#include <string>

namespace rtc {
struct ice_candidate {
  int mlineindex{};
  std::string sdp;
};
} // namespace rtc

#endif
