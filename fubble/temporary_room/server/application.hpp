#ifndef UUID_9C8C6C8E_6EA5_4166_96A7_95576EE6B0ED
#define UUID_9C8C6C8E_6EA5_4166_96A7_95576EE6B0ED

#include <boost/asio/io_context.hpp>
#include <boost/thread/future.hpp>

namespace temporary_room::rooms {
class rooms;
}
namespace temporary_room::server {
class server;
class application {
public:
  struct options {
    unsigned short port{80};
    std::string matrix_server{"localhost"};
    std::string matrix_port{"8008"};
    std::string matrix_target_prefix{"/_matrix/client/r0/"};
    struct login {
      std::string username;
      std::string password;
      boost::optional<std::string> device_id;
    };
    boost::optional<login> login_;
  }; // namespace

  virtual ~application() = default;
  virtual int get_port() const = 0;
  virtual void close() = 0;
  virtual server &get_server() = 0;
  virtual rooms::rooms &get_rooms() = 0;
  virtual boost::future<void> run() = 0;

  static std::unique_ptr<application> create(boost::asio::io_context &context,
                                             options options_);
};
} // namespace temporary_room::server

#endif
