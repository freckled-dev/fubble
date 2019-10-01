#ifndef SIGNALLING_ICE_CANDIDATE_HPP
#define SIGNALLING_ICE_CANDIDATE_HPP

#include <string>

namespace signalling {
struct ice_candidate {
  std::string sdp;
};
bool operator==(const ice_candidate &first, const ice_candidate &second);
} // namespace signalling

#endif

