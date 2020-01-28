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
  // ive no idea what the other roles describe
  BOOST_ASSERT(role == Qt::DisplayRole);
  auto index_casted = static_cast<std::size_t>(index.row());
  const participant_model *result = participants[index_casted];
  return result;
}

