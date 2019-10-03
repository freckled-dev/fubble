#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include "logging/logger.hpp"
#include "signalling/connection.hpp"
#include "signalling/json_message.hpp"
#include <boost/asio/spawn.hpp>

namespace websocket {
class connection;
}
namespace server {
class connection : public signalling::connection {
public:
  connection(websocket::connection &connection,
             signalling::json_message &message_parser);
  void run(boost::asio::yield_context context);

  void send_offer(const signalling::offer &send) final;
  void send_ice_candidate(const signalling::ice_candidate &candidate) final;
  void send_answer(const signalling::answer &answer_) final;
  void send_state_offering() final;
  void send_state_answering() final;

private:
  void send(const std::string &message);
  void send(boost::asio::yield_context yield, const std::string &message);

  websocket::connection &connection_;
  signalling::json_message &message_parser;
  logging::logger logger;
};
} // namespace server

#endif
