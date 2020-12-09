#include "connection.hpp"
#include "fubble/signaling/json_message.hpp"
#include "fubble/websocket/connection.hpp"
#include <boost/beast.hpp>
#include <fmt/format.h>

using namespace signaling::client;

connection::connection(boost::executor &executor_,
                       websocket::connection_ptr connection_moved,
                       signaling::json_message &message_parser)
    : post_executor(executor_), connection_(std::move(connection_moved)),
      message_parser(message_parser) {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (!run_promise)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "run_promise is not fulfilled";
  boost::system::system_error error{boost::asio::error::operation_aborted};
  run_promise->set_exception(error);
}

boost::future<void> connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing connection";
  return connection_->close();
}

void connection::send_registration(const signaling::registration &send_) {
  auto serialized = message_parser.serialize(send_);
  send(serialized);
}

void connection::send_want_to_negotiate() {
  want_to_negotiate negotiation{};
  auto serialized = message_parser.serialize(negotiation);
  send(serialized);
}

void connection::send_offer(const signaling::offer &send_) {
  auto serialized = message_parser.serialize(send_);
  send(serialized);
}
void connection::send_ice_candidate(const signaling::ice_candidate &candidate) {
  auto serialized = message_parser.serialize(candidate);
  send(serialized);
}
void connection::send_answer(const signaling::answer &answer_) {
  auto serialized = message_parser.serialize(answer_);
  send(serialized);
}
boost::future<void> connection::run() {
  running = true;
  read_next();
  return run_promise->get_future();
}
void connection::read_next() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "reading next message";
  decltype(run_promise)::weak_type weak_run_promise = run_promise;
  connection_->read().then(
      inline_executor, [this, weak_run_promise = std::move(weak_run_promise)](
                           auto message_future) {
        if (!weak_run_promise.lock()) {
          BOOST_ASSERT(false);
          return;
        }
        did_read(message_future);
      });
}

void connection::post_read_next() {
  post_executor.submit([this] { read_next(); });
}

void connection::did_read(boost::future<std::string> &message_future) {
  try {
    auto message = message_future.get();
    parse_message(message);
    post_read_next();
  } catch (const boost::system::system_error &error) {
    running = false;
    const auto error_code = error.code();
    auto run_promise_moved = std::move(run_promise);
    // error_code == boost::asio::error::operation_aborted
    if (error_code == boost::beast::websocket::error::closed) {
      // The WebSocket stream was gracefully closed at both endpoints
      run_promise_moved->set_value();
      return;
    }
    BOOST_LOG_SEV(this->logger, logging::severity::warning)
        << "an error occured while running, error:" << error.what();
    run_promise_moved->set_exception(error);
  } catch (const std::exception &error) {
    running = false;
    auto run_promise_moved = std::move(run_promise);
    BOOST_LOG_SEV(this->logger, logging::severity::warning)
        << "an error occured while running, error:" << error.what();
    run_promise_moved->set_exception(error);
  } catch (...) {
    BOOST_ASSERT(false);
  }
}

void connection::send(const std::string &message) {
  connection_->send(message);
}

namespace {
struct message_visitor {
  connection &connection_;
  void operator()(const signaling::offer &offer) {
    connection_.on_offer(offer);
  }
  void operator()(const signaling::answer &answer) {
    connection_.on_answer(answer);
  }
  void operator()(const signaling::ice_candidate &candidate) {
    connection_.on_ice_candidate(candidate);
  }
  void operator()(const signaling::create_offer &) {
    connection_.on_create_offer();
  }
  void operator()(const signaling::want_to_negotiate &) {
    BOOST_ASSERT_MSG(false,
                     "want_to_negotiate must not be send to client connection");
  }
  void operator()(const signaling::registration &) {
    BOOST_ASSERT_MSG(false,
                     "registration must not be send to client connection");
  }
};
} // namespace

void connection::parse_message(const std::string &message) {
  auto parsed = message_parser.parse(message);
  message_visitor visitor{*this};
  boost::apply_visitor(visitor, parsed);
}
