#include "offer.hpp"

bool signaling::operator==(const offer &first, const offer &second) {
  return first.sdp == second.sdp;
}
