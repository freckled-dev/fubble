#ifndef DI_WEBSOCKET_HPP
#define DI_WEBSOCKET_HPP

#include <boost/asio/io_context.hpp>
#include <boost/di.hpp>

namespace websocket {
class acceptor;
}
namespace di_websocket {
boost::di::injector<websocket::acceptor &>
make(boost::asio::io_context &context);
}

#endif
