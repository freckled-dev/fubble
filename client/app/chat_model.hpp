#ifndef UUID_3B21F68E_73A7_4D7C_A073_A4457CCCE1AB
#define UUID_3B21F68E_73A7_4D7C_A073_A4457CCCE1AB

#include "chat_messages_model.hpp"

namespace client {
class room;
class chat;
class chat_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      chat_messages_model *messages MEMBER messages NOTIFY messages_changed)

public:
  chat_model(room &room_, QObject *parent);

  Q_INVOKABLE void sendMessage(const QString &message);

signals:
  void messages_changed(chat_messages_model *);

protected:
  client::logger logger{"chat_model"};
  chat &chat_;
  chat_messages_model *messages;
};
} // namespace client

#endif
