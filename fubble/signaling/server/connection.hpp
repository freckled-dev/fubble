#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include "fubble/signaling/connection.hpp"
#include "fubble/signaling/json_message.hpp"
#include "fubble/signaling/logger.hpp"
#include "fubble/websocket/connection_ptr.hpp"

namespace websocket {
class connection;
}

namespace signaling::server {
class connection : public signaling::connection {
public:
  connection(boost::executor &executor, websocket::connection_ptr connection,
             signaling::json_message &message_parser);
  ~connection();
  [[nodiscard]] boost::future<void> run();

  // TODO let it return a future!
  void close() final;
  void send_offer(const signaling::offer &send) final;
  void send_ice_candidate(const signaling::ice_candidate &candidate) final;
  void send_answer(const signaling::answer &answer_) final;
  void send_do_offer() final;

private:
  void run(boost::promise<void> &&promise);
  void send(const std::string &message);

  boost::executor &executor;
  websocket::connection_ptr connection_;
  signaling::json_message &message_parser;
  signaling::logger logger{"connection"};
};
} // namespace signaling::server

#endif
