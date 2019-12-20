#ifndef UUID_5D0F470B_C872_46EB_8804_1972618A74E6
#define UUID_5D0F470B_C872_46EB_8804_1972618A74E6

#include "logging/logger.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QObject>
#include <QSettings>
#pragma GCC diagnostic pop

namespace client {
class backend : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(QString room MEMBER room NOTIFY room_changed)
public:
  backend();
  ~backend();

  Q_INVOKABLE void join(const QString &room, const QString &name);

signals:
  void name_changed(QString);
  void room_changed(QString);

protected:
  logging::logger logger;
  QSettings settings;
  QString name;
  QString room;
};
} // namespace client

#endif
