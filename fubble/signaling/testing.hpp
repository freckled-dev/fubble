#ifndef UUID_1AB9BA1B_893D_44B7_BACA_2918CBBAC222
#define UUID_1AB9BA1B_893D_44B7_BACA_2918CBBAC222

#include "fubble/signaling/client/client.hpp"
#include "fubble/utils/testing.hpp"
#include "fubble/utils/uuid.hpp"

namespace signaling::testing {
client::connect_information make_connect_information() {
  client::connect_information connect_information{false, utils::testing::server,
                                                  "80", "/api/signaling/v0/"};
  return connect_information;
}
} // namespace signaling::testing
#endif
