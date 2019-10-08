#include "connection.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace client;

connection::connection(boost::executor &executor,
                       const websocket::connection_ptr &connection,
                       signalling::json_message &message_parser)
    : executor(executor), connection_(connection),
      message_parser(message_parser) {
  // run();
}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~connection()";
}

void connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing connection";
  connection_->close().then(executor, [thiz = shared_from_this()](auto) {});
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
void connection::run() {
  connection_->read().then(
      executor, [this, thiz = shared_from_this()](auto message_future) {
        try {
          auto message = message_future.get();
          parse_message(message);
          run();
        } catch (const boost::system::system_error &error) {
          BOOST_LOG_SEV(logger, logging::severity::warning)
              << "an error occured while running, error:" << error.what();
        }
        on_closed();
      });
}
void connection::send(const std::string &message) {
  connection_->send(message).then(executor,
                                  [thiz = shared_from_this()](auto) {});
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
