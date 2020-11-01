#include "leaver.hpp"
#include "room.hpp"
#include "rooms.hpp"

using namespace client;

leaver::leaver(rooms &rooms_) : rooms_(rooms_) {}

boost::future<void> leaver::leave() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "leave";
  on_about_to_leave();
  auto to_leave = rooms_.get();
  BOOST_ASSERT(to_leave);
  return to_leave->leave().then(executor,
                                [this](auto result) { on_left(result); });
}

void leaver::on_left(boost::future<void> &result) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "left";
  // delete room from rooms, no matter if successful
  rooms_.set(nullptr);
  return result.get();
}
