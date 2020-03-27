#include "server.hpp"
#include <boost/beast/version.hpp>

using namespace temporary_room::net::server;

connection::connection(boost::asio::ip::tcp::socket &&socket)
    : stream{std::move(socket)} {}

boost::future<void> connection::do_() {
  read_next_request();
  return promise->get_future();
}

bool connection::check_error(const boost::system::error_code error) {
  if (!error)
    return false;
  auto promise_copy = promise;
  if (error == boost::beast::http::error::end_of_stream)
    promise_copy->set_value();
  else
    promise_copy->set_exception(boost::system::system_error(error));
  return true;
}

void connection::read_next_request() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "read_next_request";
  request = std::make_shared<request_type>();
  boost::beast::http::async_read(stream, buffer, *request,
                                 [this](auto error, auto) {
                                   if (check_error(error))
                                     return;
                                   on_got_request(*request);
                                 });
}

void connection::on_got_request(const request_type &request) {
  nlohmann::json content = nlohmann::json::parse(request.body());
  const auto target = request.target().to_string();
  auto response = on_request(target, content);
  response.then([this](auto result) { on_got_response(result); });
}

void connection::on_got_response(response_future &response_future_) {
  response = std::make_shared<response_type>(boost::beast::http::status::ok,
                                             request->version());
  response->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  response->set(boost::beast::http::field::content_type, "application/json");
  response->keep_alive(request->keep_alive());
  try {
    auto response_content = response_future_.get().dump();
    response->body() = response_content;
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "an error occured while getting the response, error:"
        << error.what();
    response->result(boost::beast::http::status::internal_server_error);
    nlohmann::json response_content = nlohmann::json::object();
    response_content["description"] = error.what();
    response->body() = response_content.dump();
  }
  response->prepare_payload();
  send_response();
}

void connection::send_response() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_response";
  boost::beast::http::async_write(stream, *response, [this](auto error, auto) {
    if (check_error(error))
      return;
    if (response->need_eof())
      stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
    read_next_request();
  });
}

server::server(boost::asio::io_context &context, const config &config_)
    : context(context), config_(config_) {}

void server::stop() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "server gets stopped";
  acceptor.close();
}

boost::future<void> server::run() {
  boost::packaged_task<void(boost::asio::yield_context)> task{
      [this](auto yield) { run(yield); }};
  auto result = task.get_future();
  boost::asio::spawn(context, std::move(task));
  return result;
}

void server::run(boost::asio::yield_context yield) {
  const auto address = boost::asio::ip::make_address("0.0.0.0");
  const boost::asio::ip::tcp::endpoint endpoint{address, config_.port};
  acceptor.open(endpoint.protocol());
  acceptor.set_option(boost::asio::socket_base::reuse_address(true));
  acceptor.listen();
  for (;;)
    accept(yield);
}

void server::accept(boost::asio::yield_context yield) {
  boost::asio::ip::tcp::socket socket{context};
  boost::system::error_code error;
  acceptor.async_accept(socket, yield[error]);
  if (error == boost::asio::error::operation_aborted)
    return; // stop() got called
  if (error)
    throw boost::system::system_error(error);
  do_session(std::move(socket), yield);
}

void server::do_session(boost::asio::ip::tcp::socket &&socket,
                        boost::asio::yield_context yield) {
  (void)yield;
  auto connection_ = std::make_shared<connection>(std::move(socket));
  connection_->on_request = [this](const auto &target, const auto &content) {
    return on_request(target, content);
  };
  connection_->do_().then(executor, [this, connection_](auto result) {
    // safed the reference of connection in lambda capture.
    try {
      result.get();
    } catch (const std::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << "an error occured while doing the connection, error:"
          << error.what();
    }
  });
}
