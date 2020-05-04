#include "leave_model.hpp"
#include "client/room.hpp"
#include "client/rooms.hpp"

using namespace client;

leave_model::leave_model(rooms &rooms_, QObject *parent)
    : QObject(parent), rooms_(rooms_) {}

void leave_model::leave() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "leave";
  auto to_leave = rooms_.get();
  BOOST_ASSERT(to_leave);
  to_leave->leave().then(executor, [this](auto result) { on_left(result); });
}

void leave_model::on_left(boost::future<void> &result) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "left";
  try {
    result.get();
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "failed to leave the room, what:" << error.what();
  }
  left(static_cast<int>(reason::normal));
}
