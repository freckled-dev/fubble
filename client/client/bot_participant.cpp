#include "bot_participant.hpp"

using namespace client;

bot_participant::bot_participant(session::participant &session_participant)
    : participant(session_participant) {}

bot_participant::~bot_participant() = default;

boost::future<void> bot_participant::close() {
  return boost::make_ready_future();
}

participant::videos_type bot_participant::get_videos() const { return {}; }
