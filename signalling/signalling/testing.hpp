#ifndef UUID_1AB9BA1B_893D_44B7_BACA_2918CBBAC222
#define UUID_1AB9BA1B_893D_44B7_BACA_2918CBBAC222

#include "signalling/client/client.hpp"
#include "utils/testing.hpp"
#include "utils/uuid.hpp"

namespace signalling::testing {
client::client::connect_information make_connect_information() {
  client::client::connect_information connect_information{
      false, utils::testing::server, "80", "/api/signalling/v0/"};
  return connect_information;
}
} // namespace signalling::testing
#endif
