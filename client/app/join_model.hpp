#ifndef UUID_5D0F470B_C872_46EB_8804_1972618A74E6
#define UUID_5D0F470B_C872_46EB_8804_1972618A74E6

#include "logging/logger.hpp"
#include <QObject>
#include <QSettings>

namespace client {
class joiner;
class join_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(QString room MEMBER room NOTIFY room_changed)
public:
  join_model(joiner &joiner_);
  ~join_model();

  Q_INVOKABLE void join(const QString &room, const QString &name);

signals:
  void name_changed(QString);
  void room_changed(QString);
  void joined();

protected:
  logging::logger logger;
  joiner &joiner_;
  QSettings settings;
  QString name;
  QString room;
};
} // namespace client

#endif
