#include "participants_model.hpp"
#include "client/bot_participant.hpp"
#include "client/participant.hpp"
#include "session/room.hpp"
#include <boost/assert.hpp>
#include <fmt/format.h>

using namespace client;

participants_model::participants_model(room &room_, QObject *parent)
    : QAbstractListModel(parent), room_(room_) {
  on_joins(room_.get_participants());
  signal_connections.emplace_back(room_.on_participants_join.connect(
      [this](auto joins) { on_joins(joins); }));
  signal_connections.emplace_back(room_.on_participants_left.connect(
      [this](auto leaves) { on_leaves(leaves); }));
}

int participants_model::rowCount([
    [maybe_unused]] const QModelIndex &parent) const {
  return participants.size();
}

QVariant participants_model::data([[maybe_unused]] const QModelIndex &index,
                                  [[maybe_unused]] int role) const {
  BOOST_ASSERT(role == participant_role);
  BOOST_LOG_SEV(logger, logging::severity::trace) << "getting some data";
  auto index_casted = static_cast<std::size_t>(index.row());
  client::participant_model *result = participants[index_casted].model;
  return QVariant::fromValue(result);
}

QHash<int, QByteArray> participants_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[participant_role] = "participant";
  return roles;
}

void participants_model::on_joins(
    const std::vector<participant *> &joins_to_filter) {
  std::vector<participant *> joins = filter_joining(joins_to_filter);
  if (joins.empty())
    return;
  int participants_count = participants.size();
  int joins_count = joins.size();
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << fmt::format("on_joins, participants_count:{}, joins_count:{}",
                     participants_count, joins_count);

  auto instance_participant_model = [&](participant *source) {
    participant_container result;
    result.id = source->get_id();
    result.model = new participant_model(*source, this);
    return result;
  };
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  std::transform(joins.cbegin(), joins.cend(), std::back_inserter(participants),
                 instance_participant_model);
  endInsertRows();
}

std::vector<participant *>
participants_model::filter_joining(const std::vector<participant *> &joining) {
  return filter_out_bots(joining);
}

std::vector<participant *>
participants_model::filter_out_bots(const std::vector<participant *> &joining) {
  std::vector<participant *> result;
  std::copy_if(joining.cbegin(), joining.cend(), std::back_inserter(result),
               [&](auto check) {
                 // dont do bots
                 // TODO do a class witch just holds media participants per room
                 return dynamic_cast<bot_participant *>(check) == nullptr;
               });
  return result;
}

void participants_model::on_leaves(std::vector<std::string> leaves) {
  int participants_count = participants.size();
  int leaves_count = leaves.size();
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << fmt::format("on_leaves, participants_count:{}, leaves_count:{}",
                     participants_count, leaves_count);
  for (const auto &leave : leaves) {
    auto found = std::find_if(participants.begin(), participants.end(),
                              [&](auto check) { return check.id == leave; });
    if (found == participants.end())
      continue; // bots will get skipped in this list
    int found_index = std::distance(participants.begin(), found);
    beginRemoveRows(QModelIndex(), found_index, found_index);
    found->model->deleteLater();
    participants.erase(found);
    endRemoveRows();
  }
}
