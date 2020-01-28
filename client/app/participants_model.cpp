#include "participants_model.hpp"
#include <boost/assert.hpp>

using namespace client;

participants_model::participants_model(QObject *parent)
    : QAbstractListModel(parent) {
  for (auto counter = 0; counter < 3; ++counter) {
    participants.emplace_back(new participant_model(this));
  }
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
