#ifndef SIGNALING_OFFER_HPP
#define SIGNALING_OFFER_HPP

#include <string>

namespace signaling {
struct offer {
  std::string sdp;
};
bool operator==(const offer &first, const offer &second);

} // namespace signaling

#endif
