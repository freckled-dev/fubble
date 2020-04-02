#ifndef UUID_E928967D_952B_4293_A853_EBE17F6B4F59
#define UUID_E928967D_952B_4293_A853_EBE17F6B4F59

#include "matrix/client.hpp"
#include "temporary_room/rooms/rooms.hpp"

namespace temporary_room::server {
class matrix_rooms_factory_adapter : public rooms::room_factory {
public:
  matrix_rooms_factory_adapter(matrix::client &matrix_client);
  ~matrix_rooms_factory_adapter() override;
  boost::future<rooms::room_ptr> create() override;

protected:
  matrix::client &matrix_client;
};
} // namespace temporary_room::server

#endif