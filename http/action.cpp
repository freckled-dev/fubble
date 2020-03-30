#include "action.hpp"

using namespace http;

action::action(boost::asio::io_context &context, boost::beast::http::verb verb,
               const std::string &target, const server &server_,
               const fields &fields_)
    : stream{context}, resolver{context}, server_{server_} {
  // promise is a shared ref, for the case that the promise callbacks destroy
  // this class
  promise = std::make_shared<async_result_promise>();
  std::string target_with_prefix = fields_.target_prefix + target;
  request = request_type{verb, target_with_prefix, fields_.version};
  request.set(boost::beast::http::field::host, fields_.host);
  request.set(boost::beast::http::field::user_agent, fields_.agent);
  request.set(boost::beast::http::field::accept, "application/json");
  if (fields_.auth_token)
    request.set(boost::beast::http::field::authorization,
                std::string("Bearer ") + fields_.auth_token.value());
}

void action::set_request_body(const nlohmann::json &body) {
  request.body() = body.dump();
}

action::async_result_future action::do_() {
  request.prepare_payload();
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "request:" << request;
  std::weak_ptr<int> alive = alive_check;
  resolver.async_resolve(
      server_.server, server_.port,
      [this, alive = std::move(alive)](auto error, auto result) {
        if (!alive.lock())
          return;
        on_resolved(error, result);
      });
  return promise->get_future();
}

void action::on_resolved(
    const boost::system::error_code &error,
    const boost::asio::ip::tcp::resolver::results_type &resolved) {
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "on_resolved";
  if (!check_and_handle_error(error))
    return;
  std::weak_ptr<int> alive = alive_check;
  stream.async_connect(resolved,
                       [this, alive = std::move(alive)](auto error, auto) {
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
      stream, request, [this, alive = std::move(alive)](auto error, auto) {
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
      stream, response_buffer, response,
      [this, alive = std::move(alive)](auto error, auto) {
        if (!alive.lock())
          return;
        on_response_read(error);
      });
}

void action::on_response_read(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  auto http_code = response.result();
  auto body = response.body();
  auto json_body = nlohmann::json::parse(body);
  stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
  auto promise_copy = promise;
  promise->set_value(std::make_pair(http_code, json_body));
}

bool action::check_and_handle_error(const boost::system::error_code &error) {
  if (!error)
    return true;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "got an error, error:" << error.message();
  auto promise_copy = promise;
  promise->set_exception(boost::system::system_error{error});
  return false;
}

