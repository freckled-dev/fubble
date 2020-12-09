#ifndef UUID_3B809525_60BA_4E59_9647_52C23E7A52EE
#define UUID_3B809525_60BA_4E59_9647_52C23E7A52EE

#include "chat_model.hpp"
#include "fubble/client/logger.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "participants_with_video_model.hpp"
#include <QObject>
#include <boost/thread/future.hpp>
#include <memory>

namespace client {
class room;
class room_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(bool videosAvailable MEMBER videos_available NOTIFY
                 videos_available_changed)
  Q_PROPERTY(participant_model *ownParticipant MEMBER own_participant NOTIFY
                 own_participant_changed)
  Q_PROPERTY(participants_model *participants MEMBER participants NOTIFY
                 participants_changed)
  Q_PROPERTY(participants_with_video_model *participantsWithVideo MEMBER
                 participants_with_video NOTIFY participants_with_video_changed)
  Q_PROPERTY(chat_model *chat MEMBER chat NOTIFY chat_changed)
  Q_PROPERTY(int newParticipants MEMBER new_participants_count NOTIFY
                 new_participants_count_changed)

public:
  room_model(model_creator &model_creator_, const std::shared_ptr<room> &room_,
             QObject *parent);
  ~room_model();

public slots:
  void recalculate_video_available();
  void raise_new_participants_count();
  void resetNewParticipants();

signals:
  void name_changed(QString);
  void own_participant_changed(participant_model *);
  void participants_changed(participants_model *);
  void videos_available_changed(bool);
  void participants_with_video_changed(participants_with_video_model *);
  void chat_changed(chat_model *);
  void new_participants_count_changed(int);

protected:
  void set_name();
  void set_own();

  client::logger logger{"room_model"};
  model_creator &model_creator_;
  std::shared_ptr<room> room_;
  QString name;
  bool videos_available{};
  participant_model *own_participant{};
  participants_model *participants{};
  participants_with_video_model *participants_with_video{};
  chat_model *chat{};
  int new_participants_count{};
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace client

#endif
