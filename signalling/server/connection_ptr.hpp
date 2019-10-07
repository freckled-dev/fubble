#ifndef SERVER_CONNECTION_PTR_HPP
#define SERVER_CONNECTION_PTR_HPP

#include <memory>

namespace server {
class connection;
using connection_ptr = std::shared_ptr<connection>;
} // namespace server

#endif
