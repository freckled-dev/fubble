#include "ssl_upgrader.hpp"
#include "https_connection.hpp"

using namespace http;

ssl_upgrader::ssl_upgrader(const server &server_, https_connection &connection_)
    : server_(server_), connection_(connection_) {}

ssl_upgrader::~ssl_upgrader() {
  if (!promise)
    return;
  boost::system::system_error error{boost::asio::error::operation_aborted};
  promise->set_exception(error);
}

boost::future<void> ssl_upgrader::secure_connection() {
  auto promise_copy = promise;
  // TODO verify the ssl https://github.com/djarek/certify
  // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/reference/ssl__host_name_verification.html
  connection_.get_native_ssl_context().set_default_verify_paths();
  connection_.get_native_ssl_context().set_verify_mode(
      boost::asio::ssl::verify_none);
  // TODO replace the following line with certify
  if (!SSL_set_tlsext_host_name(connection_.get_native().native_handle(),
                                server_.host.c_str())) {
    boost::beast::error_code error{static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category()};
    check_and_handle_error(error);
    return promise->get_future();
  }
  std::weak_ptr<boost::promise<void>> alive = promise;
  connection_.get_native().async_handshake(
      boost::asio::ssl::stream_base::client,
      [this, alive = std::move(alive)](const auto error) {
        if (!alive.lock())
          return;
        on_secured(error);
      });
  return promise->get_future();
}

void ssl_upgrader::on_secured(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  auto promise_moved = std::move(promise);
  promise_moved->set_value();
}

bool ssl_upgrader::check_and_handle_error(
    const boost::system::error_code &error) {
  if (!error)
    return true;
  auto promise_moved = std::move(promise);
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "got an error, error:" << error.message();
  promise_moved->set_exception(boost::system::system_error{error});
  return false;
}
