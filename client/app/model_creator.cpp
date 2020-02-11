#include "model_creator.hpp"
#include "room_model.hpp"

using namespace client;

model_creator::model_creator() = default;

room_model *model_creator::create_room_model(const std::shared_ptr<room> &room_,
                                             QObject *parent) {
  return new room_model(room_, parent);
}
