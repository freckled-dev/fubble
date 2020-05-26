#ifndef UUID_34EC7F18_25A0_46C2_9CCB_8732C2504C25
#define UUID_34EC7F18_25A0_46C2_9CCB_8732C2504C25

#include "connection_ptr.hpp"
#include <boost/asio/io_context.hpp>

namespace websocket {
class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);
  virtual ~connection_creator() = default;

  virtual connection_ptr create(bool secure);

private:
  boost::asio::io_context &context;
};
} // namespace websocket

#endif
