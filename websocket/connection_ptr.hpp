#ifndef WEBSOCKET_CONNECTIO_PTR_HPP
#define WEBSOCKET_CONNECTIO_PTR_HPP

#include <memory>

namespace websocket {
class connection;
using connection_ptr = std::unique_ptr<connection>;
} // namespace websocket

#endif
