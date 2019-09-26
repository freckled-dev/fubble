#ifndef SIGNALLING_CONNECTION_PTR_HPP
#define SIGNALLING_CONNECTION_PTR_HPP
#include <memory>

namespace signalling {
class connection;
using connection_ptr = std::shared_ptr<connection>;
} // namespace signalling
#endif
