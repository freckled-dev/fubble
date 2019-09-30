#ifndef SIGNALLING_ICE_CANDIDATE_HPP
#define SIGNALLING_ICE_CANDIDATE_HPP

#include <string>

namespace signalling {
struct ice_candidate {
  std::string sdp;
};
} // namespace signalling

#endif

