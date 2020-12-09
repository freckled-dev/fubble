#include "fubble/http/client_module.hpp"
#include "fubble/http/action_factory.hpp"
#include "fubble/http/connection_creator.hpp"
#include <boost/asio/io_context.hpp>

using namespace http;

client_module::client_module(
    std::shared_ptr<utils::executor_module> executor_module)
    : executor_module{executor_module} {}

client_module::~client_module() = default;

std::shared_ptr<connection_creator> client_module::get_connection_creator() {
  if (!connection_creator_)
    connection_creator_ = std::make_shared<connection_creator>(
        *executor_module->get_io_context());
  return connection_creator_;
}

std::shared_ptr<action_factory> client_module::get_action_factory() {
  if (!action_factory_)
    action_factory_ =
        std::make_shared<http::action_factory>(*get_connection_creator());
  return action_factory_;
}
