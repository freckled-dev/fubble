#include "client.hpp"
#include "error.hpp"
#include "room.hpp"

using namespace matrix;

client::client(room_factory &room_factory_, http::client_factory &http_factory,
               const information &information_)
    : room_factory_(room_factory_), http_factory(http_factory),
      information_(information_) {
  http_client = create_http_client();
}

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
  return http_client->post("createRoom", content)
      .then(executor, [&](auto result) {
        auto response = result.get();
        auto response_json = response.second;
        error::check_matrix_response(response.first, response_json);
        std::string room_id = response_json["room_id"];
        return room_factory_.create(*this, room_id);
      });
}

