#include "participant_model.hpp"
#include "participant.hpp"

using namespace client;

participant_model::participant_model(participant &participant_, QObject *parent)
    : QObject(parent), participant_(participant_) {
  set_name();
}

void participant_model::set_name() {
  auto name_standard = participant_.get_name();
  name = QString::fromStdString(name_standard);
  name_changed(name);
}
