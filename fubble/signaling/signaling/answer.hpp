#ifndef SIGNALING_ANSWER_HPP
#define SIGNALING_ANSWER_HPP

#include <string>

namespace signaling {
struct answer {
  std::string sdp;
};
bool operator==(const answer &first, const answer &second);

} // namespace signaling

#endif

