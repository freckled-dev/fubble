#include "chat_messages_model.hpp"
#include "client/chat.hpp"
#include "client/room.hpp"

using namespace client;

namespace {
chat_messages_model::chat_message cast_client_message(const chat::message &to_cast) {
  chat_messages_model::chat_message result;
  result.message = QString::fromStdString(to_cast.body);
  result.name = QString::fromStdString(to_cast.sender);
  result.own = to_cast.own;
  result.timestamp = QDateTime::fromTime_t(
      std::chrono::system_clock::to_time_t(to_cast.timestamp));
  return result;
}
} // namespace

chat_messages_model::chat_messages_model(room &room_, QObject *parent)
    : QAbstractListModel(parent), chat_(room_.get_chat()) {
  auto messages_to_cast = chat_.get_messages();
  std::transform(messages_to_cast.cbegin(), messages_to_cast.cend(),
                 std::back_inserter(messages), cast_client_message);
  on_message_connection = chat_.on_message.connect([this](const auto &new_) {
    auto casted = cast_client_message(new_);
    add_message(casted);
  });
}

void chat_messages_model::add_message(const chat_message &add) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  messages.push_back(add);
  endInsertRows();
}

int chat_messages_model::rowCount([
    [maybe_unused]] const QModelIndex &parent) const {
  return messages.size();
}

QVariant chat_messages_model::data(const QModelIndex &index,
                                   [[maybe_unused]] int role) const {
  const std::size_t row = static_cast<std::size_t>(index.row());
  BOOST_ASSERT(row < messages.size());
  const chat_message &message_ = messages[row];
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

QHash<int, QByteArray> chat_messages_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[role_message] = "message";
  roles[role_name] = "name";
  roles[role_own] = "own";
  roles[role_timestamp] = "timestamp";
  roles[role_type] = "type";
  return roles;
}
