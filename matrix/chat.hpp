#ifndef UUID_06D4FF02_B490_4688_827B_072E6148116A
#define UUID_06D4FF02_B490_4688_827B_072E6148116A

#include "logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <chrono>
#include <deque>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace http {
class client;
}
namespace matrix {
class client;
class user;
class chat {
public:
  chat(client &client_, const std::string &room_id);

  // m.text https://matrix.org/docs/spec/client_server/latest#m-text
  struct message {
    user *user_;
    std::chrono::system_clock::time_point timestamp;
    std::string body;
  };
  using messages = std::deque<message>;
  const messages &get_messages() const;
  boost::future<void> send(const std::string &to_send);
  boost::signals2::signal<void(const message &)> on_message;

  bool sync_event(const std::string &type, const nlohmann::json &content);

protected:
  void on_event_m_room_message(const nlohmann::json &content);

  matrix::logger logger{"chat"};
  boost::inline_executor executor;
  client &client_;
  http::client &http_client;
  const std::string room_id;
  messages messages_;
};
} // namespace matrix

#endif
