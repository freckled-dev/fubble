#include "connection.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace signalling::client;

connection::connection(boost::executor &executor_,
                       websocket::connection_ptr connection_moved,
                       signalling::json_message &message_parser)
    : post_executor(executor_), connection_(std::move(connection_moved)),
      message_parser(message_parser) {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "client ~connection()";
}

boost::future<void> connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing connection";
  return connection_->close();
}

void connection::send_registration(const signalling::registration &send_) {
  auto serialized = message_parser.serialize(send_);
  send(serialized);
}

void connection::send_want_to_negotiate() {
  want_to_negotiate negotiation{};
  auto serialized = message_parser.serialize(negotiation);
  send(serialized);
}

void connection::send_offer(const signalling::offer &send_) {
  auto serialized = message_parser.serialize(send_);
  send(serialized);
}
void connection::send_ice_candidate(
    const signalling::ice_candidate &candidate) {
  auto serialized = message_parser.serialize(candidate);
  send(serialized);
}
void connection::send_answer(const signalling::answer &answer_) {
  auto serialized = message_parser.serialize(answer_);
  send(serialized);
}
boost::future<void> connection::run() {
  running = true;
  read_next();
  return run_promise.get_future();
}
void connection::read_next() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "reading next message";
  connection_->read().then(inline_executor, [this](auto message_future) {
    try {
      auto message = message_future.get();
      parse_message(message);
      post_read_next();
    } catch (const boost::system::system_error &error) {
      running = false;
      const auto error_code = error.code();
      // error_code == boost::asio::error::operation_aborted
      if (error_code == boost::beast::websocket::error::closed) {
        // The WebSocket stream was gracefully closed at both endpoints
        run_promise.set_value();
        return;
      }
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "an error occured while running, error:" << error.what();
      run_promise.set_exception(error);
    } catch (...) {
      BOOST_ASSERT(false);
    }
  });
}

void connection::post_read_next() {
  post_executor.submit([this] { read_next(); });
}

void connection::send(const std::string &message) {
  connection_->send(message);
}

namespace {
struct message_visitor {
  connection &connection_;
  void operator()(const signalling::offer &offer) {
    connection_.on_offer(offer);
  }
  void operator()(const signalling::answer &answer) {
    connection_.on_answer(answer);
  }
  void operator()(const signalling::ice_candidate &candidate) {
    connection_.on_ice_candidate(candidate);
  }
  void operator()(const signalling::create_offer &) {
    connection_.on_create_offer();
  }
  void operator()(const signalling::want_to_negotiate &) {
    BOOST_ASSERT_MSG(false,
                     "want_to_negotiate must not be send to client connection");
  }
  void operator()(const signalling::registration &) {
    BOOST_ASSERT_MSG(false,
                     "registration must not be send to client connection");
  }
};
} // namespace

void connection::parse_message(const std::string &message) {
  auto parsed = message_parser.parse(message);
  message_visitor visitor{*this};
  std::visit(visitor, parsed);
}
