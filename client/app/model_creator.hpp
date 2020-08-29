#ifndef UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C
#define UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C

#include <QObject>
#include <memory>

namespace client {
class audio_device_settings;
class audio_level_calculator_factory;
class audio_volume;
class own_audio_information;
class participant;
class participant_model;
class participants_model;
class room;
class room_model;
class video_settings;
class model_creator {
public:
  model_creator(audio_level_calculator_factory &audio_level_calculator_factory_,
                audio_device_settings &audio_settings_,
                video_settings &video_settings_,
                own_audio_information &audio_information_,
                audio_volume &audio_volume_);
  room_model *create_room_model(const std::shared_ptr<room> &room_,
                                QObject *parent);
  participants_model *create_participants_model(room &room_, QObject *parent);
  participant_model *create_participant_model(participant &participant_,
                                              QObject *parent);

protected:
  audio_level_calculator_factory &audio_level_calculator_factory_;
  audio_device_settings &audio_settings_;
  video_settings &video_settings_;
  own_audio_information &audio_information_;
  audio_volume &audio_volume_;
};
} // namespace client

#endif
