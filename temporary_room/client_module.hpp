#ifndef UUID_4CD5DBDF_07F3_4AA2_96D9_04417C71467D
#define UUID_4CD5DBDF_07F3_4AA2_96D9_04417C71467D

#include "http/client_module.hpp"
#include <string>

namespace http {
class client;
}
namespace temporary_room::net {
class client;
}
namespace temporary_room {
class client_module {
public:
  struct config {
    std::string host = "fubble.io";
    std::string service = "https";
    bool use_ssl{true};
    std::string target = "/api/temporary_room/v0/";
  };

  client_module(std::shared_ptr<http::client_module> http_client_module,
                const config &config_);
  virtual ~client_module();

  std::shared_ptr<temporary_room::net::client> get_client();
  std::shared_ptr<http::client> get_http_client();

protected:
  std::shared_ptr<http::client_module> http_client_module;
  const config config_;
  std::shared_ptr<temporary_room::net::client> client_;
  std::shared_ptr<http::client> http_client;
};
} // namespace temporary_room

#endif
