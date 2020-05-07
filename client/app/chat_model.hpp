#ifndef UUID_3B21F68E_73A7_4D7C_A073_A4457CCCE1AB
#define UUID_3B21F68E_73A7_4D7C_A073_A4457CCCE1AB

#include "client/logger.hpp"
#include <QAbstractItemModel>
#include <QtCore/QDateTime>
#include <deque>

namespace client {
class room;
class chat_model : public QAbstractListModel {
  Q_OBJECT
public:
  chat_model(room &room_, QObject *parent);

  enum role {
    role_name = Qt::UserRole + 1,
    role_own,
    role_timestamp,
    role_message,
    role_type
  };

protected:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  struct message {
    QString name;
    bool own;
    QDateTime timestamp;
    QString message;
    QString type{"message"};
  };

  client::logger logger{"chat_model"};
  std::deque<message> messages;
};
} // namespace client

#endif
