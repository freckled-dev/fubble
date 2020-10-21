#ifndef SIGNALLING_REGISTRATION_HPP
#define SIGNALLING_REGISTRATION_HPP

#include <boost/optional.hpp>
#include <string>

namespace signalling {
struct registration {
  std::string key;
  std::string reconnect_token;
};
inline bool operator==(const registration &first, const registration &second) {
  return first.key == second.key &&
         first.reconnect_token == second.reconnect_token;
}
} // namespace signalling

#endif
