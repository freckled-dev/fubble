#include "model_creator.hpp"
#include "client/audio_settings.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "room_model.hpp"

using namespace client;

model_creator::model_creator(audio_settings &audio_settings_)
    : audio_settings_(audio_settings_) {}

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
  return new participant_model(participant_, audio_settings_, parent);
}
