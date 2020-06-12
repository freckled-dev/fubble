#ifndef UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C
#define UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C

#include <QObject>
#include <memory>

namespace client {
class room_model;
class room;
class participant;
class audio_settings;
class participants_model;
class participant_model;
class model_creator {
public:
  model_creator(audio_settings &audio_settings_);
  room_model *create_room_model(const std::shared_ptr<room> &room_,
                                QObject *parent);
  participants_model *create_participants_model(room &room_, QObject *parent);
  participant_model *create_participant_model(participant &participant_,
                                              QObject *parent);

protected:
  audio_settings &audio_settings_;
};
} // namespace client

#endif
