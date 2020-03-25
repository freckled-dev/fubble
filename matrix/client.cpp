#include "client.hpp"
#include "error.hpp"
#include "room.hpp"
#include <fmt/format.h>

using namespace matrix;

client::client(factory &factory_, http::client_factory &http_factory,
               const information &information_)
    : factory_(factory_), http_factory(http_factory),
      information_(information_) {
  http_client = create_http_client();
  users_ = factory_.create_users(*this);
}

const std::string &client::get_user_id() const { return information_.user_id; }

users &client::get_users() const { return *users_; }

std::unique_ptr<http::client> client::create_http_client() {
  auto fields = http_factory.get_fields();
  fields.auth_token = information_.access_token;
  return http_factory.create(fields);
}

boost::future<std::unique_ptr<room>> client::create_room() {
  nlohmann::json content = nlohmann::json::object();
  content["preset"] = "private_chat";
  auto creation_content = nlohmann::json::object();
  creation_content["m.federate"] = false;
  content["creation_content"] = creation_content;
  auto initial_state = nlohmann::json::array();
  auto history_visibility = nlohmann::json::object();
  history_visibility["type"] = "m.room.history_visibility";
  auto history_visibility_content = nlohmann::json::object();
  history_visibility_content["history_visibility"] = "joined";
  history_visibility["content"] = history_visibility_content;
  initial_state.push_back(history_visibility);
  content["initial_state"] = initial_state;
#if 0
  BOOST_LOG_SEV(logger, logging::severity::trace) << "fun:" << content.dump(2);
#endif
  return http_client->post("createRoom", content)
      .then(executor, [this](auto result) {
        auto response = result.get();
        auto response_json = response.second;
        error::check_matrix_response(response.first, response_json);
        std::string room_id = response_json["room_id"];
        return factory_.create_room(*this, room_id);
      });
}

boost::future<std::unique_ptr<room>>
client::join_room_by_id(const std::string &id) {
  nlohmann::json content = nlohmann::json::object();
  auto target = fmt::format("rooms/{}/join", id);
  return http_client->post(target, content).then(executor, [this](auto result) {
    auto response = result.get();
    auto response_json = response.second;
    error::check_matrix_response(response.first, response_json);
    std::string room_id = response_json["room_id"];
    return factory_.create_room(*this, room_id);
  });
}

boost::future<void> client::sync(std::chrono::milliseconds timeout) {
  std::string target = fmt::format("sync?timeout={}", timeout.count());
  if (sync_next_batch)
    target += "&since=" + sync_next_batch.value();
  return http_client->get(target).then(executor, [this](auto result) {
    auto response = result.get();
    auto response_json = response.second;
    error::check_matrix_response(response.first, response_json);
    on_synced(response_json);
  });
}

void client::on_synced(const nlohmann::json &content) {
  sync_next_batch = content["next_batch"];
#if 0
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "content:" << content.dump(2);
#endif
  auto presence = content["presence"];
#if 1
  BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
      "user_id:'{}', presence:{}", get_user_id(), presence.dump(2));
#endif
  on_sync(content);
}
