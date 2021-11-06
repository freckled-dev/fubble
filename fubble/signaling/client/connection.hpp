#ifndef SIGNALING_CLIENT_CONNECTION_HPP
#define SIGNALING_CLIENT_CONNECTION_HPP

#include "fubble/signaling/answer.hpp"
#include "fubble/signaling/ice_candidate.hpp"
#include "fubble/signaling/logger.hpp"
#include "fubble/signaling/offer.hpp"
#include "fubble/signaling/registration.hpp"
#include "fubble/websocket/connection_ptr.hpp"
#include <sigslot/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace signaling {
class json_message;

namespace client {
class connection {
public:
  connection(boost::executor &executor, websocket::connection_ptr connection,
             signaling::json_message &message_parser);
  ~connection();

  boost::future<void> close();

  void send_registration(const signaling::registration &send);
  void send_want_to_negotiate();
  void send_offer(const signaling::offer &send);
  void send_ice_candidate(const signaling::ice_candidate &candidate);
  void send_answer(const signaling::answer &answer_);

  sigslot::signal<const signaling::offer &> on_offer;
  sigslot::signal<const signaling::answer &> on_answer;
  sigslot::signal<const signaling::ice_candidate &>
      on_ice_candidate;
  sigslot::signal<> on_create_offer;

  boost::future<void> run();

private:
  void read_next();
  void post_read_next();
  void did_read(boost::future<std::string> &message_future);

  void send(const std::string &message);
  void parse_message(const std::string &message);

  signaling::logger logger{"client::connection"};
  boost::executor &post_executor;
  boost::inline_executor inline_executor;
  std::shared_ptr<boost::promise<void>> run_promise =
      std::make_shared<boost::promise<void>>();
  websocket::connection_ptr connection_;
  signaling::json_message &message_parser;
  bool running{};
};
} // namespace client
} // namespace signaling

#endif
