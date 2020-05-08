#include "chat.hpp"
#include "matrix/chat.hpp"
#include "matrix/user.hpp"

using namespace client;

namespace {
chat::message cast_matrix_message(const std::string &own_id,
                                  const matrix::chat::message &to_cast) {
  chat::message result;
  result.body = to_cast.body;
  result.sender = to_cast.user_->get_display_name();
  result.timestamp = to_cast.timestamp;
  result.own = to_cast.user_->get_id() == own_id;
  return result;
}
} // namespace

chat::chat(matrix::chat &matrix_chat, const std::string &own_id)
    : matrix_chat(matrix_chat) {
  auto current_messages = matrix_chat.get_messages();
  std::transform(current_messages.cbegin(), current_messages.cend(),
                 std::back_inserter(messages_), [own_id](const auto &item) {
                   return cast_matrix_message(own_id, item);
                 });
  on_message_connection =
      matrix_chat.on_message.connect([own_id, this](auto new_) {
        auto casted = cast_matrix_message(own_id, new_);
        add_message(casted);
      });
}

const chat::messages &chat::get_messages() const { return messages_; }

void chat::send_message(const std::string &message) {
  matrix_chat.send(message);
}

void chat::add_message(const message &add) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "add_message, from:" << add.sender;
  messages_.push_back(add);
  on_message(add);
}
