#ifndef SIGNALLING_REGISTRATION_HPP
#define SIGNALLING_REGISTRATION_HPP

#include <optional>
#include <string>

namespace signalling {
struct registration {
  std::string key;
  std::optional<std::string> token;
};
struct registration_token {
  std::string token;
};
inline bool operator==(const registration_token &first,
                       const registration_token &second) {
  return first.token == second.token;
}
} // namespace signalling

#endif
