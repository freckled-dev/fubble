#ifndef UUID_307C4AF5_CA51_47FF_9618_3CAF0AC98FAE
#define UUID_307C4AF5_CA51_47FF_9618_3CAF0AC98FAE

#include <memory>

namespace boost::asio {
class io_context;
}
namespace signalling::server {
class application {
public:
  virtual ~application() = default;
  virtual int get_port() const = 0;
  virtual void close() = 0;

  static std::unique_ptr<application> create(boost::asio::io_context &context,
                                             int port);
};
} // namespace signalling::server

#endif
