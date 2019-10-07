#include "connection.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace server;

connection::connection(const websocket::connection_ptr &connection_,
                       signalling::json_message &message_parser)
    : connection_(connection_), message_parser(message_parser) {}

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

void connection::run(boost::asio::yield_context yield) {
  while (true) {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "reading next message";
    auto message = connection_->read(yield);
    auto parsed = message_parser.parse(message);
    std::visit(message_visitor{*this}, parsed);
  }
}

void connection::send_offer(const signalling::offer &offer) {
  send(message_parser.serialize(offer));
}

void connection::send_ice_candidate(
    const signalling::ice_candidate &candidate) {
  send(message_parser.serialize(candidate));
}

void connection::send_answer(const signalling::answer &answer_) {
  send(message_parser.serialize(answer_));
}

void connection::send_state_offering() {}

void connection::send_state_answering() {}

void connection::send(const std::string &message) {
  boost::asio::spawn([this, thiz = shared_from_this(), message](auto yield) {
    send(yield, message);
  });
}

void connection::send(boost::asio::yield_context yield,
                      const std::string &message) {
  boost::system::error_code error;
  connection_->send(yield[error], message);
  if (!error)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
      "could not write message! eror message:'{}'", error.message());
}

