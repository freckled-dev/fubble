#ifndef UUID_4B5752F5_3075_468F_BF5E_661E210A6B0F
#define UUID_4B5752F5_3075_468F_BF5E_661E210A6B0F

#include "client.hpp"
#include "matrix/client.hpp"

namespace session {
class client_factory {
public:
  std::unique_ptr<client> create(std::unique_ptr<matrix::client> matrix_client);
};
} // namespace session

#endif
