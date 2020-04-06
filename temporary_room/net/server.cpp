#include "server.hpp"
#include <fmt/format.h>

using namespace temporary_room::net::server;

server::server(acceptor &net_server) : acceptor_(net_server) {
  acceptor_.on_request =
      [this](const std::string &target,
             const nlohmann::json &request) -> response_future {
    return on_request(target, request);
  };
}

response_future server::on_request(const std::string &target,
                                   const nlohmann::json &request) {
  const std::string prefix = "join";
  if (target != prefix && target != "/" + prefix)
    return boost::make_exceptional_future<nlohmann::json>(
        std::runtime_error(fmt::format("invalid target:'{}'", target)));
  const std::string user_id = request["user_id"];
  const std::string room_name = request["room_name"];
  return on_join(room_name, user_id).then([](auto result) {
    auto response = nlohmann::json::object();
    response["room_id"] = result.get();
    return response;
  });
}
