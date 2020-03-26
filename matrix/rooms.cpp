#include "rooms.hpp"
#include "client.hpp"
#include "error.hpp"
#include "factory.hpp"
#include <fmt/format.h>

using namespace matrix;

rooms::rooms(factory &factory_, client &client_)
    : factory_(factory_), client_(client_) {
  on_sync_connection = client_.on_sync.connect(
      [this](const auto &content) { on_sync(content); });
  http_client = client_.create_http_client();
}

std::optional<room *> rooms::get_room_by_id(const std::string &id) {
  auto found =
      std::find_if(rooms_.begin(), rooms_.end(),
                   [&](const auto &check) { return check->get_id() == id; });
  if (found == rooms_.end())
    return {};
  return found->get();
}

void rooms::on_sync(const nlohmann::json &content) { (void)content; }

boost::future<room *> rooms::create_room() {
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
        const std::string room_id = response_json["room_id"];
        auto room_ = factory_.create_room(client_, room_id);
        auto return_result = room_.get();
        on_room_created(std::move(room_));
        return return_result;
      });
}

boost::future<room *> rooms::join_room_by_id(const std::string &id) {
  auto target = fmt::format("rooms/{}/join", id);
  nlohmann::json content = nlohmann::json::object();
  return http_client->post(target, content).then(executor, [this](auto result) {
    auto response = result.get();
    auto response_json = response.second;
    error::check_matrix_response(response.first, response_json);
    std::string room_id = response_json["room_id"];
    auto room_ = factory_.create_room(client_, room_id);
    auto return_result = room_.get();
    on_room_created(std::move(room_));
    return return_result;
  });
}

void rooms::on_room_created(std::unique_ptr<room> room_) {
  BOOST_ASSERT(!get_room_by_id(room_->get_id()));
  rooms_.emplace_back(std::move(room_));
}
