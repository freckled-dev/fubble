#include "participant.hpp"
#include "fubble/matrix/user.hpp"

using namespace client;

participant::participant(matrix::user &matrix_participant)
    : matrix_participant(matrix_participant), id{matrix_participant.get_id()} {
  connection_update =
      matrix_participant.on_update.connect([this] { update(); });
}

participant::~participant() = default;

std::string participant::get_id() const { return id; }

std::string participant::get_name() const {
  return matrix_participant.get_display_name();
}

void participant::update() {
  on_name_changed(matrix_participant.get_display_name());
}
