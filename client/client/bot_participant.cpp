#include "bot_participant.hpp"

using namespace client;

bot_participant::bot_participant(matrix::user &matrix_participant)
    : participant(matrix_participant) {}

bot_participant::~bot_participant() = default;

boost::future<void> bot_participant::close() {
  return boost::make_ready_future();
}

participant::videos_type bot_participant::get_videos() const { return {}; }
