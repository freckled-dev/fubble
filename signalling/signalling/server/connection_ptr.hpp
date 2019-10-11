#ifndef SERVER_CONNECTION_PTR_HPP
#define SERVER_CONNECTION_PTR_HPP

#include <memory>

namespace signalling::server {
class connection;
using connection_ptr = std::shared_ptr<connection>;
} // namespace signalling::server

#endif
