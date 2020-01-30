#include "participants_model.hpp"
#include "session/room.hpp"
#include <boost/assert.hpp>

using namespace client;

participants_model::participants_model(room &room_, QObject *parent)
    : QAbstractListModel(parent), room_(room_) {
  // TODO append to room signals
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
  client::participant_model *result = participants[index_casted];
  return QVariant::fromValue(result);
}

QHash<int, QByteArray> participants_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[participant_role] = "participant";
  return roles;
}

