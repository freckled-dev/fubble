#ifndef UUID_26F21668_2EF6_4D1F_B49F_73E91D8C15BD
#define UUID_26F21668_2EF6_4D1F_B49F_73E91D8C15BD

#include "fubble/websocket/connection.hpp"

namespace websocket {
// TODO move the whole websocket connection caching to here
class caching_connection : public connection {
public:
  caching_connection();

  boost::future<void> send(const std::string &message) override;
  boost::future<std::string> read() override;
  boost::future<void> close() override;
};
} // namespace websocket

#endif
