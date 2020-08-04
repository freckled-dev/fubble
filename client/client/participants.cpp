#include "participants.hpp"
#include "matrix/room.hpp"
#include "matrix/room_participant.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"

using namespace client;

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

participants::participants(
    std::unique_ptr<participant_creator> participant_creator_parameter,
    users &users_, matrix::room &matrix_room)
    : participant_creator_{std::move(participant_creator_parameter)},
      users_(users_), matrix_room(matrix_room) {
  on_matrix_room_participant_joins(matrix_room.get_members());
  on_join_connection =
      matrix_room.on_join.connect([this](matrix::room_participant &joined) {
        std::vector<matrix::room_participant *> casted;
        casted.push_back(&joined);
        on_matrix_room_participant_joins(casted);
      });
}

participants::~participants() = default;

std::vector<participant *> participants::get_all() const {
  std::vector<participant *> result;
  std::transform(participants_.cbegin(), participants_.cend(),
                 std::back_inserter(result), [](const auto &item) {
                   BOOST_ASSERT(item);
                   return item.get();
                 });
  return result;
}

participant *participants::get(const std::string &id) const {
  auto found = find(id);
  if (found == participants_.cend())
    return nullptr;
  return found->get();
}

boost::future<void> participants::close() {
  auto leaver_ = std::make_shared<leaver>();
  std::transform(participants_.begin(), participants_.end(),
                 std::back_inserter(leaver_->futures),
                 [](auto &participant_) { return participant_->close(); });
  return leaver_->wait_for_done();
}

void participants::on_matrix_room_participant_joins(
    const std::vector<matrix::room_participant *> &joins) {
  for (auto join : joins) {
    if (join->get_join_state() == matrix::join_state::join)
      add(*join);
    join->on_join_state_changed.connect(
        [this, join] { on_join_state_changed(*join); });
  }
}

void participants::on_join_state_changed(matrix::room_participant &changed) {
  if (changed.get_join_state() == matrix::join_state::join)
    add(changed);
  if (changed.get_join_state() == matrix::join_state::leave)
    remove_by_id(changed.get_id());
}

void participants::remove_by_id(const std::string &id) {
  auto found = find(id);
  BOOST_ASSERT(found != participants_.cend());
  if (found == participants_.cend())
    return;
  participants_.erase(found);
  std::vector<std::string> signal_argument;
  signal_argument.push_back(id);
  on_removed(signal_argument);
}

void participants::add(matrix::room_participant &add_) {
  BOOST_ASSERT(find(add_.get_id()) == participants_.cend());
  auto new_participant = participant_creator_->create(add_.get_user());
  participants_.push_back(std::move(new_participant));
  std::vector<participant *> signal_argument;
  signal_argument.push_back(participants_.back().get());
  on_added(signal_argument);
}

participants::participants_container::iterator
participants::find(const std::string &id) {
  return std::find_if(participants_.begin(), participants_.end(),
                      [&](const auto &check) { return id == check->get_id(); });
}

participants::participants_container::const_iterator
participants::find(const std::string &id) const {
  return std::find_if(participants_.cbegin(), participants_.cend(),
                      [&](const auto &check) { return id == check->get_id(); });
}
