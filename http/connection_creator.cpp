#include "connection_creator.hpp"
#include "http/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

using namespace http;

namespace {
struct connector : std::enable_shared_from_this<connector> {
  http::logger logger{"connector"};
  boost::asio::io_context &context;
  boost::asio::ip::tcp::resolver resolver{context};
  boost::promise<void> promise;
  boost::beast::tcp_stream stream{context};
  boost::asio::ssl::context ssl_context{
      boost::asio::ssl::context::tlsv12_client};
  boost::beast::ssl_stream<boost::beast::tcp_stream &> ssl_stream{stream,
                                                                  ssl_context};
  const server server_;

  connector(boost::asio::io_context &context_, const server server_)
      : context(context_), server_(server_) {}

  boost::future<void> do_() {
    resolve();
    return promise.get_future();
  }

  void resolve() {
    std::weak_ptr<connector> alive = shared_from_this();
    resolver.async_resolve(
        server_.host, server_.port,
        [this, alive = std::move(alive)](auto error, auto result) {
          if (!alive.lock())
            return;
          on_resolved(error, result);
        });
  }
  void
  on_resolved(const boost::system::error_code &error,
              const boost::asio::ip::tcp::resolver::results_type &resolved) {
    // BOOST_LOG_SEV(logger, logging::severity::trace) << "on_resolved";
    if (!check_and_handle_error(error))
      return;
    std::weak_ptr<connector> alive = shared_from_this();
    stream.async_connect(resolved,
                         [this, alive = std::move(alive)](auto error, auto) {
                           if (!alive.lock())
                             return;
                           on_connected(error);
                         });
  }
  void on_connected(const boost::system::error_code &error) {
    if (!check_and_handle_error(error))
      return;
    if (server_.secure)
      return secure_connection();
    done();
  }
  void secure_connection() {
    // TODO verify the ssl https://github.com/djarek/certify
    // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/reference/ssl__host_name_verification.html
    ssl_context.set_default_verify_paths();
    ssl_context.set_verify_mode(boost::asio::ssl::verify_none);
    // TODO replace the following line with certify
    if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(),
                                  server_.host.c_str())) {
      boost::beast::error_code error{static_cast<int>(::ERR_get_error()),
                                     boost::asio::error::get_ssl_category()};
      check_and_handle_error(error);
      return;
    }
    std::weak_ptr<connector> alive = shared_from_this();
    ssl_stream.async_handshake(
        boost::asio::ssl::stream_base::client,
        [this, alive = std::move(alive)](const auto error) {
          if (!alive.lock())
            return;
          on_secured(error);
        });
  }

  void on_secured(const boost::system::error_code &error) {
    if (!check_and_handle_error(error))
      return;
    done();
  }

  bool check_and_handle_error(const boost::system::error_code &error) {
    if (!error)
      return true;
    auto self = shared_from_this(); // in case the fullfilled promise destroy
                                    // the instance
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "got an error, error:" << error.message();
    promise.set_exception(boost::system::system_error{error});
    return false;
  }

  void done() {
    auto self = shared_from_this();
    promise.set_value();
  }
};

struct connection_ssl : public connection {
  boost::beast::tcp_stream stream;
  boost::beast::ssl_stream<boost::beast::tcp_stream &> ssl_stream;

  connection_ssl(
      boost::beast::tcp_stream &&stream,
      boost::beast::ssl_stream<boost::beast::tcp_stream &> &&ssl_stream)
      : stream(std::move(stream)), ssl_stream(std::move(ssl_stream)) {}

  void cancel() override { stream.socket().close(); }

  void shutdown() override {
    stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  }

  http_or_https get_native() override { return &ssl_stream; }
};

class connection_insecure : public connection {
public:
  boost::beast::tcp_stream stream;

  connection_insecure(boost::beast::tcp_stream &&stream)
      : stream(std::move(stream)) {}

  void cancel() override { stream.socket().close(); }

  void shutdown() override {
    stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  }

  http_or_https get_native() override { return &stream; }
};

} // namespace

connection_creator::connection_creator(boost::asio::io_context &context)
    : context(context) {}

namespace {
std::unique_ptr<connection>
create_connection_from_connector(connector &connector_) {
  if (connector_.server_.secure)
    return std::make_unique<connection_insecure>(std::move(connector_.stream));
  return std::make_unique<connection_ssl>(std::move(connector_.stream),
                                          std::move(connector_.ssl_stream));
}
} // namespace

boost::future<std::unique_ptr<connection>>
connection_creator::create(const server &server_) {
  auto connector_ = std::make_shared<connector>(context, server_);
  return connector_->do_().then(executor, [connector_](auto result) {
    result.get();
    return create_connection_from_connector(*connector_);
  });
  //
}
