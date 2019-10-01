#include "ice_candidate.hpp"

bool signalling::operator==(const ice_candidate &first,
                            const ice_candidate &second) {
  return first.sdp == second.sdp;
}

