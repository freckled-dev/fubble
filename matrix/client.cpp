#include "client.hpp"
#include "error.hpp"
#include "http/action.hpp"
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

client::~client() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "destructor";
  if (!sync_till_stop_promise)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "!sync_till_stop_promise";
#if 0
  sync_till_stop_promise->set_exception(
      boost::system::system_error(boost::asio::error::operation_aborted));
#endif
}

const std::string &client::get_user_id() const { return information_.user_id; }

users &client::get_users() const { return *users_; }

rooms &client::get_rooms() const { return *rooms_; }

http::client &client::get_http_client() { return *http_client; }

std::unique_ptr<http::client> client::create_http_client() {
  auto fields = http_factory.get_fields();
  fields.auth_token = information_.access_token;
  return http_factory.create(fields);
}

boost::future<void> client::set_display_name(const std::string &name) {
  nlohmann::json request = nlohmann::json::object();
  request["displayname"] = name;
  std::string target = fmt::format("profile/{}/displayname", get_user_id());
  return http_client->put(target, request).then(executor, [](auto result) {
    error::check_matrix_response(result);
  });
}

boost::future<void> client::sync(std::chrono::milliseconds timeout) {
  auto target = make_sync_target(timeout);
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
  on_sync(content);
}

std::string client::make_sync_target(std::chrono::milliseconds timeout) const {
  std::string target = fmt::format("sync?timeout={}", timeout.count());
  if (sync_next_batch)
    target += "&since=" + sync_next_batch.value();
  return target;
}

boost::future<void> client::sync_till_stop(std::chrono::milliseconds timeout) {
  BOOST_ASSERT(!http_sync_action);
  BOOST_ASSERT(!sync_till_stop_promise);
  sync_till_stop_timeout = timeout;
  sync_till_stop_promise = std::make_unique<boost::promise<void>>();
  do_sync();
  return sync_till_stop_promise->get_future();
}

void client::do_sync() {
  auto target = make_sync_target(sync_till_stop_timeout);
  http_sync_action = http_client->get_action(target);
  http_sync_action->do_().then(
      executor, [this](auto result) { on_sync_till_stop(result); });
}

void client::on_sync_till_stop(
    boost::future<std::pair<boost::beast::http::status, nlohmann::json>>
        &result) {
  try {
    auto response = result.get();
    auto response_json = response.second;
    error::check_matrix_response(response.first, response_json);
    on_synced(response_json);
    do_sync();
  } catch (...) {
    auto error = boost::current_exception();
    sync_till_stop_promise->set_exception(error);
    sync_till_stop_promise.reset();
  }
}

void client::stop_sync() { http_sync_action->cancel(); }
