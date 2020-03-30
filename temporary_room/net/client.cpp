#include "client.hpp"
#include "http/client.hpp"
#include <boost/thread/future.hpp>
#include <fmt/format.h>

using namespace temporary_room::net;

error::error(boost::beast::http::status code, const nlohmann::json &error_)
    : message(fmt::format("code:'{}', error_json:{}", code, error_.dump())) {}

const char *error::what() const noexcept { return message.c_str(); }

client::client(http::client &http_client) : http_client(http_client) {}

boost::future<std::string> client::join(const std::string &name,
                                        const std::string &user_id) {
  auto content = nlohmann::json::object();
  content["user_id"] = user_id;
  auto target = fmt::format("join/{}", name);
  return http_client.put(target, content).then(executor, [](auto result) {
    auto got_result = result.get();
    if (got_result.first != boost::beast::http::status::ok)
      throw error(got_result.first, got_result.second);
    auto response = got_result.second;
    std::string room_id = response["room_id"];
    return room_id;
  });
}
