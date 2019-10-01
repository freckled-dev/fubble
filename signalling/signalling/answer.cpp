#include "answer.hpp"

bool signalling::operator==(const answer &first, const answer &second) {
  return first.sdp == second.sdp;
}

