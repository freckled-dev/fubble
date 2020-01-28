#include "participant_model.hpp"

using namespace client;

participant_model::participant_model(QObject *parent) : QObject(parent) {
  name = "some name, this:" + QString::number(reinterpret_cast<long>(this));
}

