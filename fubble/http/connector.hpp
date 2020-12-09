#ifndef UUID_DCEE3AD5_70DA_4351_9B44_5F25183DC2A0
#define UUID_DCEE3AD5_70DA_4351_9B44_5F25183DC2A0

#include "connection_impl.hpp"
#include "fubble/http/logger.hpp"
#include "http_connection.hpp"
#include "https_connection.hpp"
#include "ssl_upgrader.hpp"
#include <boost/thread/executors/inline_executor.hpp>

namespace http {
class connector {
public:
  connector(boost::asio::io_context &context, connection &connection_,
            const server server_)
      : context(context), server_(server_) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "connector()";
    ssl_connection = dynamic_cast<https_connection *>(&connection_);
    if (!ssl_connection) {
      tcp_connection = dynamic_cast<http_connection *>(&connection_);
      BOOST_ASSERT(tcp_connection);
      return;
    }
    tcp_connection = &ssl_connection->get_http_connection();
    ssl_upgrader_ =
        std::make_unique<ssl_upgrader<https_connection::native_type>>(
            server_, ssl_connection->get_native(),
            ssl_connection->get_native_ssl_context());
    BOOST_ASSERT(tcp_connection);
    BOOST_ASSERT(ssl_connection);
  }

  ~connector() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "~connector()";
    if (!promise)
      return;
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "connector promise not fullfilled in destructor";
    auto fullfilled = std::move(promise);
    fullfilled->set_exception(
        boost::system::system_error(boost::asio::error::operation_aborted));
  }

  boost::future<void> do_() {
    BOOST_ASSERT(!promise);
    promise = std::make_shared<boost::promise<void>>();
    resolve();
    return promise->get_future();
  }

protected:
  http_type &get_http() { return tcp_connection->get_native(); }

  void resolve() {
    std::weak_ptr<boost::promise<void>> alive = promise;
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
    // BOOST_LOG_SEV(logger, logging::severity::debug) << "on_resolved";
    if (!check_and_handle_error(error))
      return;
    std::weak_ptr<boost::promise<void>> alive = promise;
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
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "got an error, error:" << error.message();
    auto fullfill =
        std::move(promise); // move away from member, in case class gets
                            // destroyed by fullfilling the promise
    fullfill->set_exception(boost::system::system_error{error});
    return false;
  }

  void done() {
    auto fullfill = std::move(promise);
    fullfill->set_value();
  }

  http::logger logger{"connector"};
  boost::asio::io_context &context;
  boost::asio::ip::tcp::resolver resolver{context};
  std::shared_ptr<boost::promise<void>> promise;
  const server server_;
  http_connection *tcp_connection{};
  https_connection *ssl_connection{};
  std::unique_ptr<ssl_upgrader<https_connection::native_type>> ssl_upgrader_;
  boost::inline_executor executor;
};

} // namespace http
#endif
