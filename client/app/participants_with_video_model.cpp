#include "participants_with_video_model.hpp"
#include "client/bot_participant.hpp"
#include "client/own_participant.hpp"
#include "client/participant.hpp"
#include "session/room.hpp"
#include <boost/assert.hpp>
#include <fmt/format.h>

using namespace client;

std::vector<participant *> participants_with_video_model::filter_joining(
    const std::vector<participant *> &joining) {
  auto prefiltered = participants_model::filter_joining(joining);
  std::vector<participant *> result;
  std::copy_if(prefiltered.cbegin(), prefiltered.cend(),
               std::back_inserter(result),
               [&](auto check) { return !check->get_videos().empty(); });
  return result;
}

