#ifndef CLIENT_CONNECTION_PTR_HPP
#define CLIENT_CONNECTION_PTR_HPP

#include <memory>

namespace client {
class connection;
using connection_ptr = std::shared_ptr<connection>;
} // namespace client

#endif
