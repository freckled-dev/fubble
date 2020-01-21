#ifndef UUID_5D0F470B_C872_46EB_8804_1972618A74E6
#define UUID_5D0F470B_C872_46EB_8804_1972618A74E6

#include "logging/logger.hpp"
#include "ui/executor_qt.hpp"
#include <QObject>
#include <QSettings>
#include <boost/thread/future.hpp>

namespace client {
class joiner;
class room_model;
class room;
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
  void joined(client::room_model *room);

protected:
  void on_joined(boost::future<std::shared_ptr<room>> room_);

  logging::logger logger;
  joiner &joiner_;
  ui::executor_qt qt_executor;
  QSettings settings;
  QString name;
  QString room;
};
} // namespace client

#endif
