#ifndef UUID_602DF6B7_FC5A_4E8D_9CD9_EAB83FB12967
#define UUID_602DF6B7_FC5A_4E8D_9CD9_EAB83FB12967

#include "client.hpp"
#include <fubble/http/client_factory.hpp>
#include <fubble/http2/module.hpp>

namespace matrix {
class client_factory {
public:
  client_factory(
      factory &room_factory_,
      const std::shared_ptr<fubble::http2::factory> &http_client_factory);

  std::unique_ptr<client> create(const client::information &information);

protected:
  factory &room_factory_;
  const std::shared_ptr<fubble::http2::factory> http_client_factory;
};
} // namespace matrix

#endif
