#ifndef UUID_9A234102_E6DB_48C0_A9DB_0B4DA3C84747
#define UUID_9A234102_E6DB_48C0_A9DB_0B4DA3C84747

#include "connection.hpp"
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <variant>

namespace http {

using http_type = boost::beast::tcp_stream;
using ssl_type = boost::beast::ssl_stream<http_type &>;
using http_or_https = std::variant<http_type *, ssl_type *>;

struct connection_ssl : public connection {
  boost::beast::tcp_stream stream;
  boost::asio::ssl::context ssl_context{
      boost::asio::ssl::context::tlsv12_client};
  boost::beast::ssl_stream<boost::beast::tcp_stream &> ssl_stream;

  connection_ssl(boost::asio::io_context &context)
      : stream(context), ssl_stream(stream, ssl_context) {}

  void cancel() override { stream.socket().close(); }

  void shutdown() override {
    stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  }

  http_or_https get_native() { return &ssl_stream; }
};

class connection_insecure : public connection {
public:
  boost::beast::tcp_stream stream;

  connection_insecure(boost::asio::io_context &context) : stream(context) {}

  void cancel() override { stream.socket().close(); }

  void shutdown() override {
    stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  }

  http_or_https get_native() { return &stream; }
};
} // namespace http

#endif
