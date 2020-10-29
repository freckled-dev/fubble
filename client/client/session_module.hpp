#ifndef UUID_E1787C7F_9E5F_4756_9483_18AC5D05415D
#define UUID_E1787C7F_9E5F_4756_9483_18AC5D05415D

#include "matrix/module.hpp"

namespace client {
class joiner;
class leaver;
class tracks_adder;
class rooms;
class session_module {
public:
  struct config {
    // TODO
  };

  std::shared_ptr<joiner> get_joiner();
  std::shared_ptr<leaver> get_leaver();
  std::shared_ptr<tracks_adder> get_tracks_adder();
  std::shared_ptr<rooms> get_rooms();

protected:
};
} // namespace client

#endif
