#ifndef UUID_3B809525_60BA_4E59_9647_52C23E7A52EE
#define UUID_3B809525_60BA_4E59_9647_52C23E7A52EE

#include "client/logger.hpp"
#include "participants_model.hpp"
#include <QObject>
#include <boost/thread/future.hpp>
#include <memory>

namespace client {
class room;
class room_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(participants_model *participants MEMBER participants NOTIFY
                 participants_changed)

public:
  room_model(const std::shared_ptr<room> &room_, QObject *parent);

signals:
  void name_changed(QString);
  void participants_changed(participants_model *);

protected:
  void set_name();

  client::logger logger{"room_model"};
  std::shared_ptr<room> room_;
  QString name;
  participants_model *participants;
};
} // namespace client

#endif
