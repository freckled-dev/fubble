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
  if (error == boost::beast::http::error::end_of_stream) {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "end_of_stream";
    promise_copy->set_value();
  } else
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
  setup_response();

  try {
    const std::string request_body = request.body();
    nlohmann::json content = nlohmann::json::parse(request_body);
    const auto target = request.target().to_string();
    auto response = on_request(target, content);
    response.then([this](auto result) { on_got_response(result); });
  } catch (const std::exception &error) {
    send_error(error);
  }
}

void connection::send_error(const std::exception &error) {
  response->result(boost::beast::http::status::internal_server_error);
  nlohmann::json response_content = nlohmann::json::object();
  response_content["description"] = error.what();
  response->body() = response_content.dump();
  send_response();
}

void connection::setup_response() {
  response = std::make_shared<response_type>(boost::beast::http::status::ok,
                                             request->version());
  response->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  response->set(boost::beast::http::field::content_type, "application/json");
  response->keep_alive(request->keep_alive());
}

void connection::on_got_response(response_future &response_future_) {
  try {
    auto response_content = response_future_.get().dump();
    response->body() = response_content;
    send_response();
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "an error occured while getting the response, error:"
        << error.what();
    send_error(error);
  }
}

void connection::send_response() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "send_response";
  response->prepare_payload();
  boost::beast::http::async_write(stream, *response, [this](auto error, auto) {
    if (check_error(error))
      return;
    if (response->need_eof())
      stream.socket().shutdown(boost::asio::socket_base::shutdown_both);
    read_next_request();
  });
}

acceptor::acceptor(boost::asio::io_context &context, const config &config_)
    : context(context), config_(config_) {}

void acceptor::listen() {
  if (listening)
    return;
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "listening on port: " << config_.port;
  const auto address = boost::asio::ip::make_address("0.0.0.0");
  const boost::asio::ip::tcp::endpoint endpoint{address, config_.port};
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  listening = true;
}

boost::future<void> acceptor::run() {
  listen();
  run_promise = std::make_shared<boost::promise<void>>();
  accept_next();
  return run_promise->get_future();
}

void acceptor::stop() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "server gets stopped";
  acceptor_.close();
}

unsigned short acceptor::get_port() const {
  return acceptor_.local_endpoint().port();
}

void acceptor::accept_next() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "accept_next";
  socket = std::make_unique<boost::asio::ip::tcp::socket>(context);
  acceptor_.async_accept(*socket, [this](auto error) {
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "accepted, error:" << error.message();
    if (error) {
      auto promise_copy = run_promise;
      if (error == boost::asio::error::operation_aborted) {
        BOOST_LOG_SEV(logger, logging::severity::trace) << "operation_aborted";
        run_promise->set_value(); // stop() got called
      } else
        run_promise->set_exception(boost::system::system_error(error));
      return;
    }
    do_session();
    accept_next();
  });
}

void acceptor::do_session() {
  auto connection_ = std::make_shared<connection>(std::move(*socket));
  socket.reset();
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
