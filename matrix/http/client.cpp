#include "client.hpp"

using namespace matrix::http;

client::client(boost::asio::io_context &context, const server &server_,
               const default_fields &fields)
    : stream{context}, resolver{context}, server_{server_}, fields{fields} {}

client::async_result_future client::get(const std::string &target) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "get, target:" << target;
  create_promise();
  BOOST_ASSERT(!current_action);
  current_action = std::make_unique<get_action>(target);
  connect_and_do_action();
  return promise->get_future();
}

client::async_result_future client::post(const std::string &target,
                                         const nlohmann::json &content) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "post, target:" << target;
  create_promise();
  BOOST_ASSERT(!current_action);
  current_action = std::make_unique<post_action>(target, content);
  connect_and_do_action();
  return promise->get_future();
}

void client::create_promise() {
  BOOST_ASSERT(!promise);
  promise = std::make_unique<async_result_promise>();
}

void client::clear_promise_and_action() {
  BOOST_ASSERT(promise);
  BOOST_ASSERT(current_action);
  promise.reset();
  current_action.reset();
}

void client::connect_and_do_action() {
  BOOST_ASSERT(promise);
  BOOST_ASSERT(current_action);
  if (connected) {
    do_action();
    return;
  }
  std::weak_ptr<int> alive = alive_check;
  resolver.async_resolve(
      server_.server, server_.port,
      [this, alive = std::move(alive)](auto error, auto result) {
        if (!alive.lock())
          return;
        on_resolved(error, result);
      });
}

void client::do_action() { current_action->do_(*this); }

void client::on_resolved(
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

void client::on_connected(const boost::system::error_code &error) {
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "on_connected";
  if (!check_and_handle_error(error))
    return;
  connected = true;
  do_action();
}

bool client::check_and_handle_error(const boost::system::error_code &error) {
  if (!error)
    return true;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "got an error, error:" << error.message();
  promise->set_exception(boost::system::system_error{error});
  clear_promise_and_action();
  return false;
}

void client::do_request() {
  request.set(boost::beast::http::field::host, fields.host);
  request.set(boost::beast::http::field::user_agent, fields.agent);
  request.set(boost::beast::http::field::accept, "application/json");
#if 0
    if (auth_token)
      request.set(boost::beast::http::field::authorization,
              std::string("Bearer ") + auth_token.value());
#endif
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "request:" << request;
  request.prepare_payload();
  std::weak_ptr<int> alive = alive_check;
  boost::beast::http::async_write(
      stream, request, [this, alive = std::move(alive)](auto error, auto) {
        if (!alive.lock())
          return;
        on_request_send(error);
      });
}

void client::on_request_send(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  read_response();
}

void client::read_response() {
  std::weak_ptr<int> alive = alive_check;
  boost::beast::http::async_read(
      stream, response_buffer, response,
      [this, alive = std::move(alive)](auto error, auto) {
        if (!alive.lock())
          return;
        on_response_read(error);
      });
}

void client::on_response_read(const boost::system::error_code &error) {
  if (!check_and_handle_error(error))
    return;
  auto http_code = response.result();
  auto body = response.body();
  auto json_body = nlohmann::json::parse(body);
  promise->set_value(std::make_pair(http_code, json_body));
}

void client::get_action::do_(client &self) {
  std::string target_with_prefix = self.fields.target_prefix + target;
  self.request = request_type{boost::beast::http::verb::get, target_with_prefix,
                              self.fields.version};
  self.do_request();
}

void client::post_action::do_(client &self) {
  std::string target_with_prefix = self.fields.target_prefix + target;
  self.request = request_type{boost::beast::http::verb::post,
                              target_with_prefix, self.fields.version};
  self.request.body() = content.dump();
  self.do_request();
}
