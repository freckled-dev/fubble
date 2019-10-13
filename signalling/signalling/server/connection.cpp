#include "connection.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace signalling::server;

connection::connection(boost::executor &executor,
                       const websocket::connection_ptr &connection_,
                       signalling::json_message &message_parser)
    : executor(executor), connection_(connection_),
      message_parser(message_parser) {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~connection()";
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
  void operator()(const signalling::registration &registration_) {
    connection_.on_registration(registration_);
  }
  void operator()(const signalling::create_offer &candidate) {
    BOOST_ASSERT_MSG(false, "must not be received");
  }
  void operator()(const signalling::create_answer &candidate) {
    BOOST_ASSERT_MSG(false, "must not be received");
  }
};
} // namespace

boost::future<void> connection::run() {
  boost::promise<void> promise;
  auto result = promise.get_future();
  run(std::move(promise));
  return result;
}

void connection::run(boost::promise<void> &&promise) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "reading next message";
  connection_->read().then(executor, [this, promise = std::move(promise)](
                                         auto message_future) mutable {
    try {
      auto message = message_future.get();
      auto parsed = message_parser.parse(message);
      std::visit(message_visitor{*this}, parsed);
      run(std::move(promise));
    } catch (const boost::system::system_error &error) {
      on_closed();
      if (error.code() == boost::asio::error::operation_aborted)
        promise.set_value();
      else
        promise.set_exception(error);
    }
  });
}

void connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "close";
  connection_->close().then(executor, [connection_ = connection_](auto) {});
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

void connection::send_state_offering() {
  send(message_parser.serialize(signalling::create_offer{}));
}

void connection::send_state_answering() {
  send(message_parser.serialize(signalling::create_answer{}));
}

void connection::send(const std::string &message) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << fmt::format("sending message '{}'", message);
  auto future = connection_->send(message);
  future.then(executor, [connection_ = connection_](auto) {});
}
