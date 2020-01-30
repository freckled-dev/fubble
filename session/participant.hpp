#ifndef UUID_664721D3_17CF_427C_BB2A_BE79190FEB7D
#define UUID_664721D3_17CF_427C_BB2A_BE79190FEB7D

#include <string>

namespace session {
// TODO make it a non-copyable class, with signal `on_name_changed`
struct participant {
  std::string id;
  std::string name;
};
} // namespace session

#endif
