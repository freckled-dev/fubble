#ifndef UUID_41BBE839_56D6_4D3F_B3A4_7F6EE19AB84F
#define UUID_41BBE839_56D6_4D3F_B3A4_7F6EE19AB84F

#include <memory>

namespace matrix {
class room;
class client;
class room_factory {
public:
  std::unique_ptr<room> create(client &client_, const std::string &room_id);
};
} // namespace matrix

#endif
