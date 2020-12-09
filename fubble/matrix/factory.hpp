#ifndef UUID_41BBE839_56D6_4D3F_B3A4_7F6EE19AB84F
#define UUID_41BBE839_56D6_4D3F_B3A4_7F6EE19AB84F

#include <memory>
#include <string>

namespace matrix {
class room;
class rooms;
class client;
class users;
class factory {
public:
  std::unique_ptr<users> create_users(client &client_);
  std::unique_ptr<rooms> create_rooms(client &client_);
  std::unique_ptr<room> create_room(client &client_,
                                    const std::string &room_id);
};
} // namespace matrix

#endif
