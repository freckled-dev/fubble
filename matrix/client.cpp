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
  rooms_ = factory_.create_rooms(*this);
}

const std::string &client::get_user_id() const { return information_.user_id; }

users &client::get_users() const { return *users_; }

rooms &client::get_rooms() const { return *rooms_; }

std::unique_ptr<http::client> client::create_http_client() {
  auto fields = http_factory.get_fields();
  fields.auth_token = information_.access_token;
  return http_factory.create(fields);
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
#if 1
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "content:" << content.dump(2);
#endif
  auto presence = content["presence"];
#if 0
  BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
      "user_id:'{}', presence:{}", get_user_id(), presence.dump(2));
#endif
  on_sync(content);
}
