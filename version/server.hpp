#ifndef UUID_B07D6F3D_3682_486D_84BA_55DDFE035829
#define UUID_B07D6F3D_3682_486D_84BA_55DDFE035829

#include "utils/version.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

namespace version {
class server {
public:
  struct config {
    std::string address{"localhost"};
    int port{8085};
    std::string current_version{utils::version()};
    std::string minimum_version{utils::version()};
  };
  virtual ~server() = default;

  static std::unique_ptr<server>
  create(const std::shared_ptr<boost::asio::io_context> &context,
         const config &config_);
};
} // namespace version

#endif
