#include "participants_with_video_model.hpp"
#include "client/bot_participant.hpp"
#include "client/own_participant.hpp"
#include "client/participant.hpp"
#include "participants_model.hpp"
#include <boost/assert.hpp>
#include <fmt/format.h>

using namespace client;

participants_with_video_model::participants_with_video_model(
    participants_model &participants)
    : QAbstractListModel(&participants), participants(participants) {
  connect(&participants, &participants_model::rowsInserted, this,
          &participants_with_video_model::on_participants_inserted);
  connect(&participants, &participants_model::rowsAboutToBeRemoved, this,
          &participants_with_video_model::on_participants_about_to_be_removed);
}

int participants_with_video_model::rowCount([
    [maybe_unused]] const QModelIndex &parent) const {
  auto result = participants_container.size();
  return static_cast<int>(result);
}

QVariant participants_with_video_model::data(const QModelIndex &index,
                                             [[maybe_unused]] int role) const {
  BOOST_ASSERT(role == participant_role);
  auto row = static_cast<std::size_t>(index.row());
  BOOST_ASSERT(row < participants_container.size());
  auto result = participants_container[row];
  return QVariant::fromValue(result);
}

void participants_with_video_model::on_participants_inserted(
    [[maybe_unused]] const QModelIndex &parent, int first,
    [[maybe_unused]] int last) {
  // for now only one participant at a time gets inserted
  BOOST_ASSERT(first == last);
  auto &got = get_participant_from_participants_by_index(first);
  connect(&got, &participant_model::video_changed, this,
          [this, &got](ui::frame_provider_google_video_source *video) {
            if (video)
              return add_participant(got);
            return remove_participant(got);
          });
  if (got.get_video() == nullptr)
    return;
  add_participant(got);
}

void participants_with_video_model::on_participants_about_to_be_removed(
    [[maybe_unused]] const QModelIndex &parent, int first,
    [[maybe_unused]] int last) {
  BOOST_ASSERT(first == last);
  auto &got = get_participant_from_participants_by_index(first);
  remove_participant(got);
}

participant_model &
participants_with_video_model::get_participant_from_participants_by_index(
    int row) {
  auto index = participants.index(row);
  QVariant remove =
      participants.data(index, participants_model::participant_role);
  participant_model *got = remove.value<participant_model *>();
  BOOST_ASSERT(got);
  return *got;
}

void participants_with_video_model::add_participant(participant_model &add) {
  auto found = find(add.get_id());
  if (found != participants_container.cend())
    return;
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  participants_container.push_back(&add);
  endInsertRows();
}

void participants_with_video_model::remove_participant(
    participant_model &remove) {
  remove_participant(remove.get_id());
}

void participants_with_video_model::remove_participant(const std::string &id) {
  auto found = find(id);
  if (found == participants_container.cend())
    return;
  int found_index = std::distance(participants_container.begin(), found);
  beginRemoveRows(QModelIndex(), found_index, found_index);
  participants_container.erase(found);
  endRemoveRows();
  // pointer gets deleted in participants_model
}

QHash<int, QByteArray> participants_with_video_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[participant_role] = "participant";
  return roles;
}

participants_with_video_model::participants_container_type::iterator
participants_with_video_model::find(const std::string &id) {
  auto result =
      std::find_if(participants_container.begin(), participants_container.end(),
                   [&](auto check) { return check->get_id() == id; });
  return result;
}

