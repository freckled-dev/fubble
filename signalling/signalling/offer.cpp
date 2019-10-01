#include "offer.hpp"

bool signalling::operator==(const offer &first, const offer &second) {
  return first.sdp == second.sdp;
}
