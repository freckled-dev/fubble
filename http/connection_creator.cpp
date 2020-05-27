#include "connection_creator.hpp"
#include "connection_impl.hpp"
#include "http/logger.hpp"
#include "http_connection.hpp"
#include "https_connection.hpp"
#include "ssl_upgrader.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

using namespace http;

namespace {
// TODO refactor to own files. remove enable_shared_from_this
struct connector : std::enable_shared_from_this<connector> {
  http::logger logger{"connector"};
  boost::asio::io_context &context;
  boost::asio::ip::tcp::resolver resolver{context};
  boost::promise<std::unique_ptr<connection>> promise;
  const server server_;
  std::unique_ptr<http_connection> tcp_connection_owning;
  http_connection *tcp_connection;
  std::unique_ptr<https_connection> ssl_connection;
  std::unique_ptr<ssl_upgrader<https_connection::native_type>> ssl_upgrader_;
  boost::inline_executor executor;

  connector(boost::asio::io_context &context_, const server server_)
      : context(context_), server_(server_) {
    tcp_connection_owning = std::make_unique<http_connection>(context);
    tcp_connection = tcp_connection_owning.get();
    if (!server_.secure)
      return;
    ssl_connection =
        std::make_unique<https_connection>(std::move(tcp_connection_owning));
    ssl_upgrader_ =
        std::make_unique<ssl_upgrader<https_connection::native_type>>(
            server_, ssl_connection->get_native(),
            ssl_connection->get_native_ssl_context());
  }

  http_type &get_http() { return tcp_connection->get_native(); }

  boost::future<std::unique_ptr<connection>> do_() {
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
    auto &stream = get_http();
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
    BOOST_ASSERT(ssl_connection);
    ssl_upgrader_->secure_connection().then(executor, [this](auto result) {
      try {
        result.get();
        on_secured();
      } catch (const boost::system::system_error &error) {
        check_and_handle_error(error.code());
      }
    });
  }

  void on_secured() { done(); }

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
    auto result = [this]() -> std::unique_ptr<connection> {
      if (ssl_connection)
        return std::move(ssl_connection);
      return std::move(tcp_connection_owning);
    }();
    promise.set_value(std::move(result));
  }
};

} // namespace

connection_creator::connection_creator(boost::asio::io_context &context)
    : context(context) {}

boost::future<std::unique_ptr<connection>>
connection_creator::create(const server &server_) {
  auto connector_ = std::make_shared<connector>(context, server_);
  return connector_->do_().then(
      executor, [connector_](auto result) { return result.get(); });
}
