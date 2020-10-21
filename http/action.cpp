#include "action.hpp"
#include "connection_creator.hpp"
#include "connection_impl.hpp"
#include "connector.hpp"
#include "http_connection.hpp"
#include "https_connection.hpp"
#include <boost/exception/enable_current_exception.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

using namespace http;

namespace {
http_or_https get_native_from_connection(connection &connection_) {
  auto ssl = dynamic_cast<http_connection *>(&connection_);
  if (ssl)
    return &ssl->get_native();
  auto tcp = dynamic_cast<https_connection *>(&connection_);
  BOOST_ASSERT(tcp);
  return &tcp->get_native();
}
} // namespace

action::action(connection_creator &creator, boost::beast::http::verb verb,
               const std::string &target, const server &server_,
               const fields &fields_)
    : connection_creator_(creator), server_{server_}, verb(verb),
      target(target), fields_(fields_) {
  // promise is a shared ref, for the case that the promise callbacks destroy
  // this class
  promise = std::make_shared<async_result_promise>();
  std::string target_with_prefix = fields_.target_prefix + target;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << fmt::format("doing server:'{}', port:'{}', target_with_prefix:'{}'",
                     server_.host, server_.port, target_with_prefix);
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
  if (!connection_)
    return;
  connection_->cancel();
}

void action::set_request_body(const nlohmann::json &body) {
  auto &request = buffers_->request;
  request.body() = body.dump();
}

action::async_result_future action::do_() {
  auto &request = buffers_->request;
  request.prepare_payload();
#if 1
  BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
      "resolving, server:'{}', port:'{}'", server_.host, server_.port);
#endif
  connection_ = connection_creator_.create(server_);
  connector_ = connection_creator_.create_connector(*connection_, server_);
  connector_->do_().then(executor, [this](auto result) {
    try {
      result.get();
    } catch (const boost::system::system_error &error) {
      check_and_handle_error(error.code());
      return;
    }
    send_request();
  });
  return promise->get_future();
}

void action::cancel() {
  if (!connection_)
    return;
  connection_->cancel();
}

void action::send_request() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << __FUNCTION__ << ", request:" << buffers_->request;
  std::weak_ptr<int> alive = alive_check;
  auto callback = [buffers_ = buffers_, this,
                   alive = std::move(alive)](auto error, auto) {
    if (!alive.lock())
      return;
    on_request_send(error);
  };
  auto native = get_native_from_connection(*connection_);
  std::visit(
      [&](auto stream) {
        return boost::beast::http::async_write(*stream, buffers_->request,
                                               std::move(callback));
      },
      native);
}

void action::on_request_send(const boost::system::error_code &error) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << __FUNCTION__;
  if (!check_and_handle_error(error))
    return;
  read_response();
}

void action::read_response() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << __FUNCTION__;
  std::weak_ptr<int> alive = alive_check;
  auto callback = [buffers_ = buffers_, this,
                   alive = std::move(alive)](auto error, auto) {
    if (!alive.lock())
      return;
    on_response_read(error);
  };
  auto native = get_native_from_connection(*connection_);
  std::visit(
      [&](auto stream) {
        return boost::beast::http::async_read(
            *stream, buffers_->response_buffer, buffers_->response,
            std::move(callback));
      },
      native);
}

void action::on_response_read(const boost::system::error_code &error) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << __FUNCTION__ << ", response:" << buffers_->response;
  if (!check_and_handle_error(error))
    return;
  auto &response = buffers_->response;
  auto http_code = response.result();
  connection_->shutdown();
  auto promise_copy = std::move(promise);
  auto body = response.body();
  const auto content_type = response[boost::beast::http::field::content_type];
  const std::string target_with_prefix = fields_.target_prefix + target;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << fmt::format("got a response, for target'{}', code:{}, "
                     "content_type:'{}', body.size():{}",
                     target_with_prefix, static_cast<int>(http_code),
                     content_type, body.size());
  nlohmann::json json_body;
  try {
    json_body = nlohmann::json::parse(body);
  } catch (const nlohmann::json::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not json-parse the following body:" << body;
    // promise_copy->set_exception(error);
    promise_copy->set_exception(boost::current_exception());
    return;
  }
  promise_copy->set_value(std::make_pair(http_code, json_body));
}

bool action::check_and_handle_error(const boost::system::error_code &error) {
  if (!error)
    return true;
  // some servers seem to not shut down the ssl connection correctly. so we got
  // to ignore this error.
  // https://github.com/boostorg/beast/issues/824#issuecomment-338412225
  if (error == boost::asio::ssl::error::stream_truncated) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__
        << ", boost::asio::ssl::error::stream_truncated, going to ignore the "
           "error";
    return true;
  }
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "got an error, error:" << error.message();
  auto promise_copy = std::move(promise);
  promise_copy->set_exception(boost::system::system_error{error});
  return false;
}
