#include "server.hpp"

using namespace temporary_room::net::server;

server::server(acceptor &net_server) : acceptor_(net_server) {
  acceptor_.on_request =
      [this](const std::string &target,
             const nlohmann::json &request) -> response_future {
    const std::string user_id = request["user_id"];
    return on_request(target, user_id);
  };
}

response_future server::on_request(const std::string &target,
                                   const std::string &user_id) {
  const std::string prefix = "join/";
  if (target.rfind(prefix, 0) != 0)
    return boost::make_exceptional_future<nlohmann::json>(
        std::runtime_error("invalid target"));
  const auto name = target.substr(target.find("/") + 1);
  return on_join(name, user_id).then([](auto result) {
    auto response = nlohmann::json::object();
    response["room_id"] = result.get();
    return response;
  });
}
