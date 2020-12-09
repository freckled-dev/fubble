#include "chat_model.hpp"
#include "fubble/client/chat.hpp"
#include "fubble/client/room.hpp"
#include <boost/algorithm/string/replace.hpp>

using namespace client;

chat_model::chat_model(room &room_, QObject *parent)
    : QObject(parent), chat_(room_.get_chat()) {
  messages = new chat_messages_model(room_, this);
  connect(messages, &chat_messages_model::rowsInserted, this,
          &chat_model::raise_new_message_count);
}

void chat_model::sendMessage(const QString &message) {
  auto casted = message.toStdString();
  // markdown does not make a newline unless there is a "  " or "\" at the end
  // of the line
  auto with_new_lines =
      boost::algorithm::replace_all_copy(casted, "\n", "  \n");
  chat_.send_message(with_new_lines);
}

void chat_model::resetNewMessages() {
  new_message_count = 0;
  new_message_count_changed(new_message_count);
}

void chat_model::raise_new_message_count() {
  ++new_message_count;
  new_message_count_changed(new_message_count);
}
