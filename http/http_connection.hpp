#ifndef UUID_232AEFCA_657E_4946_840A_5C968581002E
#define UUID_232AEFCA_657E_4946_840A_5C968581002E

#include "connection.hpp"
#include <boost/beast/core/tcp_stream.hpp>

namespace http {
class http_connection : public connection {
public:
  http_connection(boost::asio::io_context &context) : stream(context) {}

  void cancel() override { stream.socket().close(); }

  void shutdown() override {
    stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  }

  boost::beast::tcp_stream &get_native() { return stream; }

protected:
  boost::beast::tcp_stream stream;
};
} // namespace http

#endif
