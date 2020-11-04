#ifndef SIGNALING_REGISTRATION_HPP
#define SIGNALING_REGISTRATION_HPP

#include <boost/optional.hpp>
#include <string>

namespace signaling {
struct registration {
  std::string key;
  std::string reconnect_token;
};
inline bool operator==(const registration &first, const registration &second) {
  return first.key == second.key &&
         first.reconnect_token == second.reconnect_token;
}
} // namespace signaling

#endif
