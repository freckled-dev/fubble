#ifndef SIGNALLING_ANSWER_HPP
#define SIGNALLING_ANSWER_HPP

#include <string>

namespace signalling {
struct answer {
  std::string sdp;
};
bool operator==(const answer &first, const answer &second);

} // namespace signalling

#endif

