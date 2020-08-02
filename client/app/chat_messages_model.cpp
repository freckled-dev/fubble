#include "chat_messages_model.hpp"
#include "client/bot_participant.hpp"
#include "client/chat.hpp"
#include "client/participant.hpp"
#include "client/participants.hpp"
#include "client/room.hpp"
#include "client/users.hpp"

using namespace client;

namespace {
chat_messages_model::chat_message
cast_client_message(const chat::message &to_cast) {
  chat_messages_model::chat_message result;
  result.message = QString::fromStdString(to_cast.body);
  result.name = QString::fromStdString(to_cast.sender);
  result.own = to_cast.own;
  result.participant_id = QString::fromStdString(to_cast.sender);
  result.timestamp = QDateTime::fromTime_t(
      std::chrono::system_clock::to_time_t(to_cast.timestamp));
  return result;
}
} // namespace

chat_messages_model::chat_messages_model(room &room_, QObject *parent)
    : QAbstractListModel(parent), chat_(room_.get_chat()),
      participants_(room_.get_participants()), users_(room_.get_users()) {
  auto messages_to_cast = chat_.get_messages();
  std::transform(messages_to_cast.cbegin(), messages_to_cast.cend(),
                 std::back_inserter(messages), cast_client_message);
  on_message_connection = chat_.on_message.connect([this](const auto &new_) {
    auto casted = cast_client_message(new_);
    add_message(casted);
  });
  participants_.on_added.connect(
      [this](const auto &added) { on_participants_added(added); });
  participants_.on_removed.connect(
      [this](const auto &removed) { on_participants_removed(removed); });
}

void chat_messages_model::add_message(const chat_message &add) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  messages.push_back(add);
  endInsertRows();
}

void chat_messages_model::on_participants_added(
    const std::vector<participant *> &added) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", added.size():" << added.size();
  if (added.empty())
    return;
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  for (auto add : added) {
    if (dynamic_cast<bot_participant *>(add))
      continue;
    chat_message casted;
    casted.type = "join";
    casted.participant_id = QString::fromStdString(add->get_id());
    casted.name = QString::fromStdString(add->get_name());
    messages.push_back(std::move(casted));
  }
  endInsertRows();
}

void chat_messages_model::on_participants_removed(
    const std::vector<std::string> &removed) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", removed.size():" << removed.size();
  if (removed.empty())
    return;
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  for (const auto &remove : removed) {
    chat_message casted;
    casted.type = "leave";
    casted.participant_id = QString::fromStdString(remove);
    const auto &user = users_.get_by_id(remove);
    casted.name = QString::fromStdString(user.name);
    messages.push_back(std::move(casted));
  }
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
  case participant_id:
    return QVariant::fromValue(message_.participant_id);
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
  roles[participant_id] = "participantId";
  return roles;
}
