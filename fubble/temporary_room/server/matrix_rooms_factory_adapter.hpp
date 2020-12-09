#ifndef UUID_E928967D_952B_4293_A853_EBE17F6B4F59
#define UUID_E928967D_952B_4293_A853_EBE17F6B4F59

#include "fubble/matrix/client.hpp"
#include "temporary_room/rooms/rooms.hpp"

namespace temporary_room::server {
// TODO there's a bug. if there is an invited user, but no joined ones, the room
// won't get closed
class matrix_rooms_factory_adapter : public rooms::room_factory {
public:
  matrix_rooms_factory_adapter(matrix::client &matrix_client);
  ~matrix_rooms_factory_adapter() override;
  void create(const std::string &room_name) override;

protected:
  void on_room_joined(matrix::room &room);
  void on_room_created(boost::future<matrix::room *> &room);

  matrix::client &matrix_client;
  temporary_room::logger logger{"matrix_rooms_factory_adapter"};
  std::vector<boost::signals2::scoped_connection> signals_connections;
};
} // namespace temporary_room::server

#endif
