#ifndef SIGNALLING_OFFER_HPP
#define SIGNALLING_OFFER_HPP

#include <string>

namespace signalling {
struct offer {
  std::string sdp;
};
bool operator==(const offer &first, const offer &second);

} // namespace signalling

#endif
