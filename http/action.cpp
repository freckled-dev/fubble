#include "action.hpp"
#include <fmt/format.h>

using namespace http;

action::action(boost::asio::io_context &context, boost::beast::http::verb verb,
               const std::string &target, const server &server_,
               const fields &fields_)
    : stream{context}, resolver{context}, server_{server_}, verb(verb),
      target(target), fields_(fields_) {
  // promise is a shared ref, for the case that the promise callbacks destroy
  // this class
  promise = std::make_shared<async_result_promise>();
  std::string target_with_prefix = fields_.target_prefix + target;
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
  BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
      "resolving, server:'{}', port:'{}'", server_.server, server_.port);
  std::weak_ptr<int> alive = alive_check;
  resolver.async_resolve(server_.server, server_.port,
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
  std::weak_ptr<int> alive = alive_check;
  boost::beast::http::async_write(
      stream, buffers_->request,
      [buffers_ = buffers_, this, alive = std::move(alive)](auto error, auto) {
        if (!alive.lock())
          return;
        on_request_send(error);
      });
}

void action::on_request_send(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  read_response();
}

void action::read_response() {
  std::weak_ptr<int> alive = alive_check;
  boost::beast::http::async_read(
      stream, buffers_->response_buffer, buffers_->response,
      [buffers_ = buffers_, this, alive = std::move(alive)](auto error, auto) {
        if (!alive.lock())
          return;
        on_response_read(error);
      });
}

void action::on_response_read(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  auto &response = buffers_->response;
  auto http_code = response.result();
  stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  auto promise_copy = std::move(promise);
  if (http_code != boost::beast::http::status::ok)
    return promise_copy->set_exception(error_not_status_200(http_code));
  auto body = response.body();
  auto json_body = nlohmann::json::parse(body);
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
