#ifndef UUID_CB55A92E_2D77_4D9A_A961_11E825878AB1
#define UUID_CB55A92E_2D77_4D9A_A961_11E825878AB1

#include "http/client_module.hpp"
#include <string>

namespace http {
class client_factory;
}
namespace matrix {
class factory;
class client_factory;
class client_synchronizer;
class authentification;
class FUBBLE_PUBLIC module {
public:
  struct config {
    std::string host = "fubble.io";
    std::string service = "https";
    bool use_ssl = true;
    std::string target = "/api/matrix/v0/_matrix/client/r0/";
  };

  module(std::shared_ptr<utils::executor_module> executor_module,
         std::shared_ptr<http::client_module> http_module,
         const config &config_);
  virtual ~module();

  std::shared_ptr<factory> get_factory();
  std::shared_ptr<client_factory> get_client_factory();
  std::shared_ptr<authentification> get_authentification();
  std::shared_ptr<http::client_factory> get_http_client_factory();
  std::shared_ptr<client_synchronizer> get_client_synchronizer();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<http::client_module> http_module;
  const config config_;
  std::shared_ptr<factory> factory_;
  std::shared_ptr<client_factory> client_factory_;
  std::shared_ptr<authentification> authentification_;
  std::shared_ptr<http::client_factory> http_client_factory;
  std::shared_ptr<client_synchronizer> client_synchronizer_;
};
} // namespace matrix

#endif
