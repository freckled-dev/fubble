#ifndef UUID_45F26CC2_10B4_469A_9B1C_A1B7907692F3
#define UUID_45F26CC2_10B4_469A_9B1C_A1B7907692F3

#include "http_connection.hpp"
#include <boost/beast/ssl.hpp>

namespace http {
class https_connection : public connection {
public:
  https_connection(std::unique_ptr<http_connection> http_connection_move)
      : http_connection_(std::move(http_connection_move)),
        ssl_stream{http_connection_->get_native(), ssl_context} {}

  void cancel() override { http_connection_->cancel(); }
  void shutdown() override { http_connection_->shutdown(); }

  using native_type = boost::beast::ssl_stream<boost::beast::tcp_stream &>;
  native_type &get_native() { return ssl_stream; }
  boost::asio::ssl::context &get_native_ssl_context() { return ssl_context; }

protected:
  std::unique_ptr<http_connection> http_connection_;
  boost::asio::ssl::context ssl_context{
      boost::asio::ssl::context::tlsv12_client};
  boost::beast::ssl_stream<boost::beast::tcp_stream &> ssl_stream;
};
} // namespace http
#endif
