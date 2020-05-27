#ifndef UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7
#define UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7

#include "http/logger.hpp"
#include "server.hpp"
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/thread/future.hpp>

namespace http {
template <class connection_type> class ssl_upgrader {
public:
  ssl_upgrader(const server &server_, connection_type &connection_,
               boost::asio::ssl::context &ssl_context)
      : server_(server_), connection_(connection_), ssl_context(ssl_context) {}
  ~ssl_upgrader() {
    if (!promise)
      return;
    boost::system::system_error error{boost::asio::error::operation_aborted};
    promise->set_exception(error);
  }

  boost::future<void> secure_connection() {
    auto promise_copy = promise;
    // TODO verify the ssl https://github.com/djarek/certify
    // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/reference/ssl__host_name_verification.html
    ssl_context.set_default_verify_paths();
    ssl_context.set_verify_mode(boost::asio::ssl::verify_none);
    // TODO replace the following line with certify
    if (!SSL_set_tlsext_host_name(connection_.native_handle(),
                                  server_.host.c_str())) {
      boost::beast::error_code error{static_cast<int>(::ERR_get_error()),
                                     boost::asio::error::get_ssl_category()};
      check_and_handle_error(error);
      return promise->get_future();
    }
    std::weak_ptr<boost::promise<void>> alive = promise;
    connection_.async_handshake(
        boost::asio::ssl::stream_base::client,
        [this, alive = std::move(alive)](const auto error) {
          if (!alive.lock())
            return;
          on_secured(error);
        });
    return promise->get_future();
  }

protected:
  void on_secured(const boost::system::error_code &error) {
    if (!check_and_handle_error(error))
      return;
    auto promise_moved = std::move(promise);
    promise_moved->set_value();
  }
  bool check_and_handle_error(const boost::system::error_code &error) {
    if (!error)
      return true;
    auto promise_moved = std::move(promise);
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "got an error, error:" << error.message();
    promise_moved->set_exception(boost::system::system_error{error});
    return false;
  }

  http::logger logger{"ssl_upgrader"};
  const server server_;
  connection_type &connection_;
  boost::asio::ssl::context &ssl_context;
  std::shared_ptr<boost::promise<void>> promise =
      std::make_shared<boost::promise<void>>();
};
} // namespace http

#endif
