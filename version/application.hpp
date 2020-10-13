#ifndef UUID_B07D6F3D_3682_486D_84BA_55DDFE035829
#define UUID_B07D6F3D_3682_486D_84BA_55DDFE035829

#include "utils/version.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

namespace version {
class application {
public:
  struct config {
    std::string address{"localhost"};
    int port{8085};
    std::string current_version{utils::version()};
    std::string minimum_version{utils::version()};
  };
  virtual ~application() = default;

  static std::unique_ptr<application>
  create(const std::shared_ptr<boost::asio::io_context> &context,
         const config &config_);
};
} // namespace version

#endif
