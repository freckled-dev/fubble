#include "connection.hpp"
#include "websocket/connection.hpp"
#include <fmt/format.h>

using namespace server;

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
};
} // namespace

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
      auto parsed = message_parser.parse(message);
      std::visit(message_visitor{*this}, parsed);
      run(std::move(promise));
    } catch (const boost::system::system_error &error) {
      if (error.code() == boost::asio::error::operation_aborted) {
        promise.set_value();
        return;
      }
      promise.set_exception(error);
    }
  });
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
  auto future = connection_->send(message);
  future.then(executor, [connection_ = connection_](auto) {});
}

