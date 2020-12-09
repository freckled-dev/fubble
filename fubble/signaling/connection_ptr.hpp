#ifndef SIGNALING_CONNECTION_PTR_HPP
#define SIGNALING_CONNECTION_PTR_HPP
#include <memory>

namespace signaling {
class connection;
using connection_ptr = std::shared_ptr<connection>;
} // namespace signaling
#endif
