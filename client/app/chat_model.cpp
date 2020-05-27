#include "chat_model.hpp"
#include "client/chat.hpp"
#include "client/room.hpp"

using namespace client;

chat_model::chat_model(room &room_, QObject *parent)
    : QObject(parent), chat_(room_.get_chat()) {
  messages = new chat_messages_model(room_, this);
  connect(messages, &chat_messages_model::rowsInserted, this,
          &chat_model::raise_new_message_count);
}

void chat_model::sendMessage(const QString &message) {
  chat_.send_message(message.toStdString());
}

void chat_model::resetNewMessages() {
  new_message_count = 0;
  new_message_count_changed(new_message_count);
}

void chat_model::raise_new_message_count() {
  ++new_message_count;
  new_message_count_changed(new_message_count);
}
