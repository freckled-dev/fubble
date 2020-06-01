#include "client.hpp"
#include "action.hpp"
#include "action_factory.hpp"
#include <nlohmann/json.hpp>

using namespace http;

client::client(action_factory &action_factory_,
               const std::pair<server, fields> &server_and_fields)
    : client(action_factory_, server_and_fields.first,
             server_and_fields.second) {}

client::client(action_factory &action_factory_, const server &server_,
               const fields &fields_)
    : action_factory_(action_factory_), server_{server_}, fields_{fields_} {}

client::~client() {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << "destructor";
#endif
}

client::async_result_future client::get(const std::string &target) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "get, target:" << target;
  auto action_ = action_factory_.create(server_, target, fields_,
                                        boost::beast::http::verb::get);
  return do_action(std::move(action_));
}

std::unique_ptr<action> client::get_action(const std::string &target) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "get, target:" << target;
  return action_factory_.create(server_, target, fields_,
                                boost::beast::http::verb::get);
}

client::async_result_future client::post(const std::string &target,
                                         const nlohmann::json &content) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "post, target:" << target;
  auto action_ = action_factory_.create(server_, target, fields_,
                                        boost::beast::http::verb::post);
  action_->set_request_body(content);
  return do_action(std::move(action_));
}

client::async_result_future client::put(const std::string &target,
                                        const nlohmann::json &content) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "put, target:" << target;
  auto action_ = action_factory_.create(server_, target, fields_,
                                        boost::beast::http::verb::put);
  action_->set_request_body(content);
  return do_action(std::move(action_));
}

client::async_result_future
client::do_action(std::unique_ptr<action> &&action_) {
  std::shared_ptr shared = std::move(action_);
  add_action(shared);
  return shared->do_().then(executor,
                            [this, action_ptr = shared.get()](auto result) {
                              remove_action(action_ptr);
                              return result.get();
                            });
}

void client::add_action(const std::shared_ptr<action> &action_) {
  actions.push_back(action_);
}
void client::remove_action(const action *action_) {
  auto found =
      std::find_if(actions.begin(), actions.end(),
                   [&](const auto &check) { return action_ == check.get(); });
  BOOST_ASSERT(found != actions.end());
  actions.erase(found);
}
