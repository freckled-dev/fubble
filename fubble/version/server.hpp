#ifndef UUID_B07D6F3D_3682_486D_84BA_55DDFE035829
#define UUID_B07D6F3D_3682_486D_84BA_55DDFE035829

#include "fubble/utils/version.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

namespace version {
class server {
public:
  struct config {
    std::string address{"0.0.0.0"};
    int port{};
    std::string current_version{utils::version()};
    std::string minimum_version{utils::version()};
  };
  virtual ~server() = default;
  virtual int get_port() const = 0;

  static std::unique_ptr<server>
  create(const std::shared_ptr<boost::asio::io_context> &context,
         const config &config_);
  static std::unique_ptr<server>
  create(const std::shared_ptr<boost::asio::io_context> &context);
  static std::unique_ptr<server> create(boost::asio::io_context &context);
};
} // namespace version

#endif
