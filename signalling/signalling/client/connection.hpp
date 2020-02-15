#ifndef SIGNALLING_CLIENT_CONNECTION_HPP
#define SIGNALLING_CLIENT_CONNECTION_HPP

#include "signalling/answer.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/logger.hpp"
#include "signalling/offer.hpp"
#include "signalling/registration.hpp"
#include "websocket/connection_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace signalling {
class json_message;

namespace client {
class connection {
public:
  connection(boost::executor &executor, websocket::connection_ptr connection,
             signalling::json_message &message_parser);
  ~connection();

  void close();

  void send_registration(const signalling::registration &send);
  void send_want_to_negotiate();
  void send_offer(const signalling::offer &send);
  void send_ice_candidate(const signalling::ice_candidate &candidate);
  void send_answer(const signalling::answer &answer_);

  boost::signals2::signal<void(const signalling::offer &)> on_offer;
  boost::signals2::signal<void(const signalling::answer &)> on_answer;
  boost::signals2::signal<void(const signalling::ice_candidate &)>
      on_ice_candidate;
  boost::signals2::signal<void()> on_create_offer;

  boost::future<void> run();

private:
  void read_next();
  void post_read_next();

  void send(const std::string &message);
  void parse_message(const std::string &message);

  signalling::logger logger{"connection"};
  boost::executor &post_executor;
  boost::inline_executor inline_executor;
  boost::promise<void> run_promise;
  websocket::connection_ptr connection_;
  signalling::json_message &message_parser;
  bool running{};
};
} // namespace client
} // namespace signalling

#endif
