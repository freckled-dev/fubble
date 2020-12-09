#include "answer.hpp"

bool signaling::operator==(const answer &first, const answer &second) {
  return first.sdp == second.sdp;
}

