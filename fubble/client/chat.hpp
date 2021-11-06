#ifndef UUID_09E21F62_6DF5_44C5_A15F_EFBD87FDF840
#define UUID_09E21F62_6DF5_44C5_A15F_EFBD87FDF840

#include "logger.hpp"
#include <fubble/utils/signal.hpp>
#include <chrono>
#include <deque>
#include <string>

namespace matrix {
class chat;
}

namespace client {
class chat {
public:
  chat(matrix::chat &matrix_chat, const std::string &own_id);

  struct message {
    bool own;
    std::string sender;
    std::chrono::system_clock::time_point timestamp;
    std::string body;
  };
  using messages = std::deque<message>;
  const messages &get_messages() const;
  utils::signal::signal<const message &> on_message;

  void send_message(const std::string &message);

protected:
  void add_message(const message &add);

  client::logger logger{"chat"};
  matrix::chat &matrix_chat;
  messages messages_;

  utils::signal::scoped_connection on_message_connection;
};
} // namespace client

#endif
