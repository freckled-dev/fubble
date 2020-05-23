#ifndef UUID_5800120C_7BB1_4257_A829_71A589906F28
#define UUID_5800120C_7BB1_4257_A829_71A589906F28

#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <variant>

namespace http {

class connection {
public:
  virtual ~connection() = default;
  virtual void shutdown() = 0;
  virtual void cancel() = 0;
  using http_type = boost::beast::tcp_stream;
  using ssl_type = boost::beast::ssl_stream<http_type &>;
  using http_or_https = std::variant<http_type *, ssl_type *>;
  virtual http_or_https get_native() = 0;
};

class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);

  boost::future<std::unique_ptr<connection>> create(const server &server_);

protected:
  boost::asio::io_context &context;
  boost::inline_executor executor;
};
} // namespace http

#endif
