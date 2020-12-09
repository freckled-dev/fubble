#include "ice_candidate.hpp"

bool signaling::operator==(const ice_candidate &first,
                            const ice_candidate &second) {
  return first.sdp == second.sdp && first.mid == second.mid &&
         first.mlineindex == second.mlineindex;
}
