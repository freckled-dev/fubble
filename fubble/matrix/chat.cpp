#include "chat.hpp"
#include "client.hpp"
#include "error.hpp"
#include "fubble/http/client.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using namespace matrix;

chat::chat(client &client_, const std::string &room_id)
    : client_(client_), http_client(client_.get_http_client()),
      room_id(room_id) {}

const chat::messages &chat::get_messages() const { return messages_; }

boost::future<void> chat::send(const std::string &to_send) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "sending a chat message with the size:" << to_send.size();
  BOOST_ASSERT(!to_send.empty());
  auto transaction_id = client_.create_transaction_id();
  auto target =
      fmt::format("rooms/{}/send/m.room.message/{}", room_id, transaction_id);
  auto request = nlohmann::json::object();
  request["msgtype"] = "m.text";
  request["body"] = to_send;
  return http_client.put(target, request).then(executor, [](auto result) {
    error::check_matrix_response(result);
  });
}

bool chat::sync_event(const std::string &type, const nlohmann::json &content) {
  if (type == "m.room.message") {
    on_event_m_room_message(content);
    return true;
  }
  return false;
}

void chat::on_event_m_room_message(const nlohmann::json &event) {
  // https://matrix.org/docs/spec/client_server/latest#m-text
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_event_m_room_message";
  auto content = event["content"];
  message result;
  result.body = content["body"];
  std::int64_t timestamp_milliseconds = event["origin_server_ts"];
  std::chrono::milliseconds timestamp_milliseconds_casted{
      timestamp_milliseconds};
  result.timestamp =
      std::chrono::system_clock::time_point{timestamp_milliseconds_casted};
  std::string user_id = event["sender"];
  auto &user_ = client_.get_users().get_or_add_user(user_id);
  result.user_ = &user_;
  messages_.push_back(result);
  on_message(result);
}
