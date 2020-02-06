#include "model_creator.hpp"
#include "room_model.hpp"

using namespace client;

model_creator::model_creator(boost::executor &backend_thread)
    : backend_thread(backend_thread) {}

  room_model *
  model_creator::create_room_model(const std::shared_ptr<room> &room_,
                                   QObject *parent) {
    return new room_model(backend_thread, room_, parent);
  }

