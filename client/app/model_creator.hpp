#ifndef UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C
#define UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C

#include <QObject>
#include <memory>

namespace client {
class audio_settings;
class own_audio_information;
class participant;
class participant_model;
class participants_model;
class room;
class room_model;
class model_creator {
public:
  model_creator(audio_settings &audio_settings_,
                own_audio_information &audio_information_);
  room_model *create_room_model(const std::shared_ptr<room> &room_,
                                QObject *parent);
  participants_model *create_participants_model(room &room_, QObject *parent);
  participant_model *create_participant_model(participant &participant_,
                                              QObject *parent);

protected:
  audio_settings &audio_settings_;
  own_audio_information &audio_information_;
};
} // namespace client

#endif
