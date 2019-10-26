#ifndef RTC_ICE_CANDIDATE_HPP
#define RTC_ICE_CANDIDATE_HPP

#include <ostream>
#include <string>

namespace rtc {
struct ice_candidate {
  int mlineindex{};
  std::string sdp;
};
std::ostream &operator<<(std::ostream &out, const ice_candidate &print);
} // namespace rtc

#endif
