#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include "signalling/connection.hpp"
#include "signalling/json_message.hpp"
#include "signalling/logger.hpp"
#include "websocket/connection_ptr.hpp"

namespace websocket {
class connection;
}

namespace signalling::server {
class connection : public signalling::connection {
public:
  connection(boost::executor &executor, websocket::connection_ptr connection,
             signalling::json_message &message_parser);
  ~connection();
  [[nodiscard]] boost::future<void> run();

  // TODO let it return a future!
  void close() final;
  void send_offer(const signalling::offer &send) final;
  void send_ice_candidate(const signalling::ice_candidate &candidate) final;
  void send_answer(const signalling::answer &answer_) final;
  void send_do_offer() final;
  void send_registration_token(const signalling::registration_token &) final;

private:
  void run(boost::promise<void> &&promise);
  void send(const std::string &message);

  boost::executor &executor;
  websocket::connection_ptr connection_;
  signalling::json_message &message_parser;
  signalling::logger logger{"connection"};
};
} // namespace signalling::server

#endif
