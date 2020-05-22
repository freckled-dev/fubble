#include "action.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

using namespace http;

action::action(boost::asio::io_context &context, boost::beast::http::verb verb,
               const std::string &target, const server &server_,
               const fields &fields_)
    : stream{context},
#if FUBBLE_ENABLE_SSL
      ssl_context{boost::asio::ssl::context::tlsv12_client},
      ssl_stream{stream, ssl_context},
#endif
      resolver{context}, server_{server_}, verb(verb), target(target),
      fields_(fields_) {
#if !FUBBLE_ENABLE_SSL
  BOOST_ASSERT(!server_.secure);
#endif
  // promise is a shared ref, for the case that the promise callbacks destroy
  // this class
  promise = std::make_shared<async_result_promise>();
  std::string target_with_prefix = fields_.target_prefix + target;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "doing target_with_prefix:" << target_with_prefix;
  auto &request = buffers_->request;
  request = request_type{verb, target_with_prefix, fields_.version};
  request.set(boost::beast::http::field::host, fields_.host);
  request.set(boost::beast::http::field::user_agent, fields_.agent);
  request.set(boost::beast::http::field::accept, "application/json");
  if (fields_.auth_token)
    request.set(boost::beast::http::field::authorization,
                std::string("Bearer ") + fields_.auth_token.value());
}

action::~action() {
  if (!promise)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
      "action is getting destructed before done, target:{}", target);
  promise->set_exception(
      boost::system::system_error(boost::asio::error::operation_aborted));
  stream.cancel();
}

void action::set_request_body(const nlohmann::json &body) {
  auto &request = buffers_->request;
  request.body() = body.dump();
}

action::async_result_future action::do_() {
  auto &request = buffers_->request;
  request.prepare_payload();
#if 0
  BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
      "resolving, server:'{}', port:'{}'", server_.host, server_.port);
#endif
  std::weak_ptr<int> alive = alive_check;
  resolver.async_resolve(server_.host, server_.port,
                         [buffers_ = buffers_, this,
                          alive = std::move(alive)](auto error, auto result) {
                           if (!alive.lock())
                             return;
                           on_resolved(error, result);
                         });
  return promise->get_future();
}

void action::cancel() { stream.socket().close(); }

void action::on_resolved(
    const boost::system::error_code &error,
    const boost::asio::ip::tcp::resolver::results_type &resolved) {
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "on_resolved";
  if (!check_and_handle_error(error))
    return;
  std::weak_ptr<int> alive = alive_check;
  stream.async_connect(resolved, [buffers_ = buffers_, this,
                                  alive = std::move(alive)](auto error, auto) {
    if (!alive.lock())
      return;
    on_connected(error);
  });
}

void action::on_connected(const boost::system::error_code &error) {
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "on_connected";
  if (!check_and_handle_error(error))
    return;
  BOOST_ASSERT(!server_.secure);
  if (server_.secure)
    return secure_connection();
  send_request();
}

void action::secure_connection() {
#if FUBBLE_ENABLE_SSL
  // TODO verify the ssl https://github.com/djarek/certify
  // https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/reference/ssl__host_name_verification.html
  ssl_context.set_verify_mode(boost::asio::ssl::verify_none);
  // TODO replace the following line with certify
  if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(),
                                server_.host.c_str())) {
    boost::beast::error_code error{static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category()};
    check_and_handle_error(error);
    return;
  }
  std::weak_ptr<int> alive = alive_check;
  ssl_stream.async_handshake(
      boost::asio::ssl::stream_base::client,
      [this, alive = std::move(alive)](const auto error) {
        if (!alive.lock())
          return;
        on_secured(error);
      });
#endif
}

void action::on_secured(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  send_request();
}

void action::send_request() {
  std::weak_ptr<int> alive = alive_check;
  auto callback = [buffers_ = buffers_, this,
                   alive = std::move(alive)](auto error, auto) {
    if (!alive.lock())
      return;
    on_request_send(error);
  };
#if FUBBLE_ENABLE_SSL
  if (server_.secure)
    return boost::beast::http::async_write(ssl_stream, buffers_->request,
                                           std::move(callback));
#endif
  boost::beast::http::async_write(stream, buffers_->request,
                                  std::move(callback));
}

void action::on_request_send(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  read_response();
}

void action::read_response() {
  std::weak_ptr<int> alive = alive_check;
  auto callback = [buffers_ = buffers_, this,
                   alive = std::move(alive)](auto error, auto) {
    if (!alive.lock())
      return;
    on_response_read(error);
  };
#if FUBBLE_ENABLE_SSL
  if (server_.secure)
    return boost::beast::http::async_read(ssl_stream, buffers_->response_buffer,
                                          buffers_->response,
                                          std::move(callback));
#endif
  boost::beast::http::async_read(stream, buffers_->response_buffer,
                                 buffers_->response, std::move(callback));
}

void action::on_response_read(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  auto &response = buffers_->response;
  auto http_code = response.result();
  stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  auto promise_copy = std::move(promise);
  auto body = response.body();
  const auto content_type = response[boost::beast::http::field::content_type];
  const std::string target_with_prefix = fields_.target_prefix + target;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << fmt::format("got a response, for target'{}', code:{}, "
                     "content_type:'{}', body.size():{}",
                     target_with_prefix, static_cast<int>(http_code),
                     content_type, body.size());
  nlohmann::json json_body;
  try {
    json_body = nlohmann::json::parse(body);
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not json-parse the following body:" << body;
    std::rethrow_exception(std::current_exception());
  }
  promise_copy->set_value(std::make_pair(http_code, json_body));
}

bool action::check_and_handle_error(const boost::system::error_code &error) {
  if (!error)
    return true;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "got an error, error:" << error.message();
  auto promise_copy = std::move(promise);
  promise_copy->set_exception(boost::system::system_error{error});
  return false;
}
