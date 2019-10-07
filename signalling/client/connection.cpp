#include "connection.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace client;

connection::connection(boost::asio::io_context &context,
                       const websocket::connection_ptr &connection,
                       signalling::json_message &message_parser)
    : context(context), connection_(connection),
      message_parser(message_parser) {
  boost::asio::spawn([this](auto yield) { run(yield); });
}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~connection()";
}

void connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "closing connection";
  BOOST_LOG_SEV(logger, logging::severity::info) << "debug 0";
  boost::asio::spawn(context, [this, thiz = shared_from_this()](auto yield) {
    BOOST_LOG_SEV(logger, logging::severity::info) << "debug 1";
    connection_->close(yield);
    BOOST_LOG_SEV(logger, logging::severity::info) << "closed connection";
  });
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
void connection::run(boost::asio::yield_context yield) {
  try {
    while (true) {
      BOOST_LOG_SEV(logger, logging::severity::trace) << "reading next message";
      auto message = connection_->read(yield);
      parse_message(message);
    }
  } catch (const boost::system::system_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << fmt::format("connection::run got an error:'{}'", error.what());
    on_closed();
  }
}
void connection::send(const std::string &message) {
  boost::asio::spawn(
      context, [this, thiz = shared_from_this(), message](auto yield) {
        try {
          connection_->send(yield, message);
        } catch (boost::system::system_error &error) {
          logging::logger logger;
          BOOST_LOG_SEV(logger, logging::severity::warning)
              << fmt::format("connection::run got an error:'{}'", error.what());
        }
      });
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
