#include "client.hpp"
#include "action.hpp"

using namespace matrix::http;

client::client(boost::asio::io_context &context, const server &server_,
               const fields &fields_)
    : context(context), server_{server_}, fields_{fields_} {}

client::~client() = default;

client::async_result_future client::get(const std::string &target) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "get, target:" << target;
  auto action_ = std::make_shared<action>(
      context, boost::beast::http::verb::get, target, server_, fields_);
  return do_action(action_);
}

client::async_result_future client::post(const std::string &target,
                                         const nlohmann::json &content) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "get, target:" << target;
  auto action_ = std::make_shared<action>(
      context, boost::beast::http::verb::post, target, server_, fields_);
  action_->set_request_body(content);
  return do_action(action_);
}

client::async_result_future
client::do_action(std::shared_ptr<action> &action_) {
  add_action(action_);
  return action_->do_().then(executor,
                             [this, action_ptr = action_.get()](auto result) {
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
