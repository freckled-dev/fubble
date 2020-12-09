#ifndef UUID_54138699_A916_4166_9274_61D4F66DA327
#define UUID_54138699_A916_4166_9274_61D4F66DA327

#include "http/client_module.hpp"
#include <string>

namespace version {
class getter;
class FUBBLE_PUBLIC client_module {
public:
  struct config {
    std::string host = "fubble.io";
    std::string service = "https";
    bool use_ssl{true};
    std::string target = "/api/version/v0/";
  };
  client_module(std::shared_ptr<http::client_module> http_client_module,
                const config &config_);

  std::shared_ptr<getter> get_getter();

protected:
  std::shared_ptr<http::client_module> http_client_module;
  const config config_;

  std::shared_ptr<getter> version_getter;
};
} // namespace version

#endif
