#include "room.hpp"
#include "chat.hpp"
#include "matrix/client.hpp"
#include "matrix/room.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"
#include <fmt/format.h>

using namespace client;

room::room(std::unique_ptr<participant_creator> participant_creator_parameter,
           std::unique_ptr<matrix::client> client_parameter,
           matrix::room &room_parameter)
    : logger{fmt::format("room:{}", room_parameter.get_id())},
      participant_creator_(std::move(participant_creator_parameter)),
      client_(std::move(client_parameter)),
      room_(room_parameter), chat_{std::make_unique<chat>(
                                 room_.get_chat(), client_->get_user_id())} {
  on_session_participant_joins(room_.get_members());
  room_.on_join.connect([this](matrix::user &joined) {
    std::deque<matrix::user *> casted;
    casted.push_back(&joined);
    on_session_participant_joins(casted);
  });
  room_.on_leave.connect(
      [this](const auto leaves) { on_session_participant_leaves({leaves}); });
  room_.on_name_changed.connect(
      [this](const auto &name) { on_name_changed(name); });
}

room::~room() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "destructor";
}

std::vector<participant *> room::get_participants() const {
  std::vector<participant *> result;
  std::transform(participants_.cbegin(), participants_.cend(),
                 std::back_inserter(result), [](const auto &item) {
                   BOOST_ASSERT(item);
                   return item.get();
                 });
  return result;
}

std::string room::get_name() const { return room_.get_name(); }

std::string room::get_own_id() const { return client_->get_user_id(); }

namespace {
struct leaver : std::enable_shared_from_this<leaver> {
  client::logger logger{"room::leaver"};
  using futures_type = std::vector<boost::future<void>>;
  futures_type futures;
  futures_type::iterator current;
  boost::promise<void> promise;
  boost::inline_executor executor;

  boost::future<void> wait_for_done() {
    if (futures.empty())
      return boost::make_ready_future();
    current = futures.begin();
    do_next();
    return promise.get_future();
  }

private:
  void do_next() {
    auto self = shared_from_this();
    current->then(executor, [this, self](auto result) {
      try {
        result.get();
      } catch (const std::exception &error_) {
        BOOST_LOG_SEV(this->logger, logging::severity::error)
            << "could not close participant, error:" << error_.what();
      }
      current = ++current;
      if (current == futures.end()) {
        promise.set_value();
        return;
      }
      do_next();
    });
  }
};
} // namespace

boost::future<void> room::leave() {
  BOOST_ASSERT(client_);
  client_->stop_sync(); // no more updates
  auto leaver_ = std::make_shared<leaver>();
  std::transform(participants_.begin(), participants_.end(),
                 std::back_inserter(leaver_->futures),
                 [](auto &participant_) { return participant_->close(); });
  return leaver_->wait_for_done()
      .then(executor,
            [this](auto result) {
              result.get();
              return client_->get_rooms().leave_room(room_);
            })
      .unwrap();
}

chat &room::get_chat() const { return *chat_; }

void room::on_session_participant_joins(
    const std::deque<matrix::user *> &joins) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_session_participant_joins, count:" << joins.size();
  std::vector<participant *> signal_joins;
  for (auto join : joins) {
    BOOST_ASSERT(find(join->get_id()) == participants_.end());
    auto participant = participant_creator_->create(*join);
    signal_joins.emplace_back(participant.get());
    participants_.emplace_back(std::move(participant));
  }
  if (signal_joins.empty())
    return;
  on_participants_join(signal_joins);
}
void room::on_session_participant_leaves(
    const std::vector<std::string> &leaves) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_session_participant_leaves, count:" << leaves.size();
  for (const auto &leave : leaves) {
    auto found = find(leave);
    if (found == participants_.end()) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "participant not in list for removal, id:" << leave;
      BOOST_ASSERT(false);
      continue;
    }
    participants_.erase(found);
  }
  on_participants_left(leaves);
}

room::participants::iterator room::find(const std::string &id) {
  return std::find_if(participants_.begin(), participants_.end(),
                      [&](auto &check) { return check->get_id() == id; });
}
