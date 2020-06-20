#ifndef UUID_FC90FCBC_7D76_4157_AB90_EA2E070E85BC
#define UUID_FC90FCBC_7D76_4157_AB90_EA2E070E85BC

#include "client/logger.hpp"
#include <QAbstractItemModel>
#include <QtCore/QDateTime>
#include <boost/signals2/connection.hpp>
#include <deque>

namespace client {
class room;
class chat;
class chat_messages_model : public QAbstractListModel {
  Q_OBJECT
public:
  chat_messages_model(room &room_, QObject *parent);

  enum role {
    role_name = Qt::UserRole + 1,
    role_own,
    role_timestamp,
    role_message,
    role_type,
    participant_id
  };

  // Don't call this struct `message`, or msvc will fail.
  struct chat_message {
    QString participant_id;
    QString name;
    bool own;
    QDateTime timestamp;
    QString message;
    QString type{"message"};
  };

protected:
  void add_message(const chat_message &add);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  client::logger logger{"chat_messages_model"};
  chat &chat_;
  std::deque<chat_message> messages;
  boost::signals2::scoped_connection on_message_connection;
};
} // namespace client

#endif
