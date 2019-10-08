#include "connection.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace client;

connection::connection(boost::executor &executor,
                       const websocket::connection_ptr &connection,
                       signalling::json_message &message_parser)
    : executor(executor), connection_(connection),
      message_parser(message_parser) {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "client ~connection()";
}

void connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing connection";
  connection_->close().then(executor, [connection_ = connection_](auto) {});
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
  boost::promise<void> promise;
  auto result = promise.get_future();
  run(std::move(promise));
  return result;
}
void connection::run(boost::promise<void> &&promise) {
  connection_->read().then(executor, [this, promise = std::move(promise)](
                                         auto message_future) mutable {
    try {
      auto message = message_future.get();
      parse_message(message);
      run(std::move(promise));
    } catch (const boost::system::system_error &error) {
      on_closed();
      if (error.code() == boost::asio::error::operation_aborted) {
        promise.set_value();
        return;
      }
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << "an error occured while running, error:" << error.what();
      promise.set_exception(error);
    }
  });
}

void connection::send(const std::string &message) {
  connection_->send(message).then(executor,
                                  [connection_ = connection_](auto) {});
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
};
} // namespace

void connection::parse_message(const std::string &message) {
  auto parsed = message_parser.parse(message);
  message_visitor visitor{*this};
  std::visit(visitor, parsed);
}
