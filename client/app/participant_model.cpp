#include "participant_model.hpp"

using namespace client;

participant_model::participant_model(QObject *parent) : QObject(parent) {
  name = "markus";
}
