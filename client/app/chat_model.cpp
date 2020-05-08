#include "chat_model.hpp"
#include "client/chat.hpp"
#include "client/room.hpp"

using namespace client;

chat_model::chat_model(room &room_, QObject *parent)
    : QObject(parent), chat_(room_.get_chat()) {
  messages = new chat_messages_model(room_, this);
}

void chat_model::sendMessage(const QString &message) {
  chat_.send_message(message.toStdString());
}

