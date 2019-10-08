#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include "logging/logger.hpp"
#include "signalling/connection.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_ptr.hpp"

namespace websocket {
class connection;
}
namespace server {
class connection : public signalling::connection,
                   public std::enable_shared_from_this<connection> {
public:
  connection(boost::executor &executor,
             const websocket::connection_ptr &connection,
             signalling::json_message &message_parser);
  ~connection();
  boost::future<void> run();

  void send_offer(const signalling::offer &send) final;
  void send_ice_candidate(const signalling::ice_candidate &candidate) final;
  void send_answer(const signalling::answer &answer_) final;
  void send_state_offering() final;
  void send_state_answering() final;

private:
  void run(boost::promise<void> &&promise);
  void send(const std::string &message);

  boost::executor &executor;
  websocket::connection_ptr connection_;
  signalling::json_message &message_parser;
  logging::logger logger;
};
} // namespace server

#endif
