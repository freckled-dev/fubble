#include "room.hpp"
#include "chat.hpp"
#include "matrix/client.hpp"
#include "matrix/room.hpp"
#include "matrix/room_participant.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"
#include "participants.hpp"
#include "users.hpp"
#include <fmt/format.h>

using namespace client;

room::room(std::unique_ptr<participant_creator> participant_creator_parameter,
           std::unique_ptr<matrix::client> client_parameter,
           matrix::room &room_parameter)
    : logger{fmt::format("room:{}", room_parameter.get_id())},
      client_(std::move(client_parameter)),
      room_(room_parameter), chat_{std::make_unique<chat>(
                                 room_.get_chat(), client_->get_user_id())},
      users_{std::make_unique<users>(client_->get_users())},
      participants_{std::make_unique<participants>(
          std::move(participant_creator_parameter), *users_, room_)} {
  room_.on_name_changed.connect(
      [this](const auto &name) { on_name_changed(name); });
}

room::~room() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "destructor";
}

boost::optional<std::string> room::get_name() const { return room_.get_name(); }

std::string room::get_own_id() const { return client_->get_user_id(); }

boost::future<void> room::leave() {
  BOOST_ASSERT(client_);
  client_->stop_sync(); // no more updates
  return participants_->close()
      .then(executor,
            [this](auto result) {
              result.get();
              return client_->get_rooms().leave_room(room_);
            })
      .unwrap();
}

chat &room::get_chat() const { return *chat_; }

participants &room::get_participants() const { return *participants_; }

users &room::get_users() const { return *users_; }

matrix::room &room::get_native() const { return room_; }
