#include "client.hpp"
#include "error.hpp"
#include "http/action.hpp"
#include "room.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

using namespace matrix;

client::client(factory &factory_, http::client_factory &http_factory,
               const information &information_)
    : logger{fmt::format("client:{}", information_.user_id)},
      factory_(factory_), http_factory(http_factory),
      information_(information_) {
  http_client = create_http_client();
  users_ = factory_.create_users(*this);
  rooms_ = factory_.create_rooms(*this);
}

client::~client() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (!sync_till_stop_promise)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "sync_till_stop_promise not nullptr";
  sync_till_stop_active = false;
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

std::string client::create_transaction_id() {
  ++transaction_id_counter;
  return std::to_string(transaction_id_counter);
}

boost::future<void> client::set_display_name(const std::string &name) {
  nlohmann::json request = nlohmann::json::object();
  request["displayname"] = name;
  std::string target = fmt::format("profile/{}/displayname", get_user_id());
  return http_client->put(target, request).then(executor, [](auto result) {
    error::check_matrix_response(result);
  });
}

boost::future<void> client::set_presence(presence set) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting presence to:" << set;
  nlohmann::json request = nlohmann::json::object();
  std::string presence_casted = fmt::format("{}", set);
  request["presence"] = presence_casted;
  std::string target = fmt::format("presence/{}/status", get_user_id());
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
  BOOST_LOG_SEV(logger, logging::severity::debug)
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
  BOOST_ASSERT(!sync_till_stop_active);
  sync_till_stop_active = true;
  sync_till_stop_timeout = timeout;
  sync_till_stop_promise = std::make_unique<boost::promise<void>>();
  do_sync();
  return sync_till_stop_promise->get_future();
}

void client::do_sync() {
  auto target = make_sync_target(sync_till_stop_timeout);
  BOOST_ASSERT(!http_sync_action);
  http_sync_action = http_client->get_action(target);
  http_sync_action->do_().then(
      executor, [this](auto result) { on_sync_till_stop(result); });
}

void client::on_sync_till_stop(
    boost::future<std::pair<boost::beast::http::status, nlohmann::json>>
        &result) {
  if (!sync_till_stop_active) {
    auto moved = std::move(sync_till_stop_promise);
    moved->set_value();
    return;
  }
  try {
    auto response = result.get();
    // don't destroy http_sync_action before result.get. In case the descructor
    // destroies the result, this would lead else to double-free of
    // http_sync_action
    auto action_done = std::move(http_sync_action);
    auto response_json = response.second;
    error::check_matrix_response(response.first, response_json);
    on_synced(response_json);
    do_sync();
  } catch (...) {
    auto error = boost::current_exception();
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "an error occured while parsing the sync response!";
    sync_till_stop_promise->set_exception(error);
    sync_till_stop_promise.reset();
    sync_till_stop_active = false;
    http_sync_action.reset();
  }
}

void client::stop_sync() {
  if (!sync_till_stop_active) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << __FUNCTION__ << ", stopping sync although not syncing.";
    return;
  }
  sync_till_stop_active = false;
  if (!http_sync_action) // may be nullptr if is getting called from
                         // on_sync_till_stop callback
    return;
  http_sync_action->cancel();
}
