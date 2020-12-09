#ifndef SIGNALING_ICE_CANDIDATE_HPP
#define SIGNALING_ICE_CANDIDATE_HPP

#include <string>

namespace signaling {
struct ice_candidate {
  int mlineindex{};
  std::string mid;
  std::string sdp;
};
bool operator==(const ice_candidate &first, const ice_candidate &second);
} // namespace signaling

#endif

