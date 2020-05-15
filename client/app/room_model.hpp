#ifndef UUID_3B809525_60BA_4E59_9647_52C23E7A52EE
#define UUID_3B809525_60BA_4E59_9647_52C23E7A52EE

#include "chat_model.hpp"
#include "client/logger.hpp"
#include "participants_with_video_model.hpp"
#include <QObject>
#include <boost/thread/future.hpp>
#include <memory>

namespace client {
class room;
class room_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
   Q_PROPERTY(participant_model *ownParticipant MEMBER own_participant NOTIFY
                 own_participant_changed)
  Q_PROPERTY(participants_model *participants MEMBER participants NOTIFY
                 participants_changed)
  Q_PROPERTY(participants_with_video_model *participantsWithVideo MEMBER
                 participants_with_video NOTIFY participants_with_video_changed)
  Q_PROPERTY(chat_model *chat MEMBER chat NOTIFY chat_changed);

public:
  room_model(const std::shared_ptr<room> &room_, QObject *parent);

signals:
  void name_changed(QString);
  void own_participant_changed(participant_model *);
  void participants_changed(participants_model *);
  void participants_with_video_changed(participants_with_video_model *);
  void chat_changed(chat_model *);

protected:
  void set_name();

  client::logger logger{"room_model"};
  std::shared_ptr<room> room_;
  QString name;
  participant_model *own_participant;
  participants_model *participants;
  participants_with_video_model *participants_with_video;
  chat_model *chat;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace client

#endif
