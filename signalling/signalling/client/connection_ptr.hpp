#ifndef CLIENT_CONNECTION_PTR_HPP
#define CLIENT_CONNECTION_PTR_HPP

#include <memory>

namespace signalling {
namespace client {
class connection;
using connection_ptr = std::unique_ptr<connection>;
} // namespace client
} // namespace signalling

#endif
