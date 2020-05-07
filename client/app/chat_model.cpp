#include "chat_model.hpp"

using namespace client;

chat_model::chat_model(room &room_, QObject *parent)
    : QAbstractListModel(parent) {}

int chat_model::rowCount([[maybe_unused]] const QModelIndex &parent) const {
  return messages.size();
}

QVariant chat_model::data(const QModelIndex &index,
                          [[maybe_unused]] int role) const {
  const std::size_t row = static_cast<std::size_t>(index.row());
  BOOST_ASSERT(row < messages.size());
  const message &message_ = messages[row];
  switch (role) {
  case role_name:
    return QVariant::fromValue(message_.name);
  case role_own:
    return QVariant::fromValue(message_.own);
  case role_type:
    return QVariant::fromValue(message_.type);
  case role_message:
    return QVariant::fromValue(message_.message);
  case role_timestamp:
    return QVariant::fromValue(message_.timestamp);
  default:
    BOOST_ASSERT(false);
  }
  return QVariant();
}

QHash<int, QByteArray> chat_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[role_message] = "message";
  roles[role_name] = "name";
  roles[role_own] = "own";
  roles[role_timestamp] = "timestamp";
  roles[role_type] = "type";
  return roles;
}
