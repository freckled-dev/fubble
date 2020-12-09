#include "model_creator.hpp"
#include "fubble/client/audio_device_settings.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "room_model.hpp"

using namespace client;

model_creator::model_creator(
    audio_level_calculator_factory &audio_level_calculator_factory_,
    audio_device_settings &audio_settings_, video_settings &video_settings_,
    own_audio_information &audio_information_, audio_volume &audio_volume_,
    std::shared_ptr<mute_deaf_communicator> muted_deaf_communicator_)
    : audio_level_calculator_factory_(audio_level_calculator_factory_),
      audio_settings_(audio_settings_), video_settings_(video_settings_),
      audio_information_(audio_information_),
      audio_volume_(audio_volume_), muted_deaf_communicator_{
                                        muted_deaf_communicator_} {}

room_model *model_creator::create_room_model(const std::shared_ptr<room> &room_,
                                             QObject *parent) {
  return new room_model(*this, room_, parent);
}

participants_model *model_creator::create_participants_model(room &room_,
                                                             QObject *parent) {
  return new participants_model(*this, room_, parent);
}

participant_model *
model_creator::create_participant_model(participant &participant_,
                                        QObject *parent) {
  return new participant_model(audio_level_calculator_factory_, participant_,
                               audio_settings_, video_settings_,
                               audio_information_, audio_volume_,
                               muted_deaf_communicator_, parent);
}
