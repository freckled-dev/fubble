#ifndef UUID_3B809525_60BA_4E59_9647_52C23E7A52EE
#define UUID_3B809525_60BA_4E59_9647_52C23E7A52EE

#include "logging/logger.hpp"
#include <QObject>
#include <memory>

namespace client {
class room;
class room_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
public:
  room_model(const std::shared_ptr<room> &room_, QObject *parent);

signals:
  void name_changed(QString);

protected:
  void get_name();

  logging::logger logger;
  std::shared_ptr<room> room_;
  QString name;
};
} // namespace client

#endif
