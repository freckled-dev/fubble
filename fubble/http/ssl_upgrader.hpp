#ifndef UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7
#define UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7

#include "add_windows_root_certs.hpp"
#include "fubble/http/logger.hpp"
#include "server.hpp"
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/predef/os/windows.h>
#include <boost/thread/future.hpp>

namespace http {
namespace internal {
// https://stackoverflow.com/questions/28264313/ssl-certificates-and-boost-asio
template <typename Verifier> class verbose_verification {
public:
  verbose_verification(Verifier verifier) : verifier_(verifier) {}

  bool operator()(bool preverified, boost::asio::ssl::verify_context &ctx) {
    char subject_name[256];
    X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    bool verified = verifier_(preverified, ctx);
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "Verifying: " << subject_name << ", Verified: " << verified;
    return verified;
  }

private:
  http::logger logger{"ssl_verification"};
  Verifier verifier_;
};

template <typename Verifier>
verbose_verification<Verifier> make_verbose_verification(Verifier verifier) {
  return verbose_verification<Verifier>(verifier);
}
} // namespace internal
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
    // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/reference/ssl__host_name_verification.html
#if BOOST_OS_WINDOWS
    add_windows_root_certs(ssl_context);
#else
    ssl_context.set_default_verify_paths();
#endif
    connection_.set_verify_mode(boost::asio::ssl::verify_peer);
    connection_.set_verify_callback(internal::make_verbose_verification(
        boost::asio::ssl::host_name_verification(server_.host)));
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
    BOOST_LOG_SEV(logger, logging::severity::debug)
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
