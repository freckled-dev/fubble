#include "action_factory.hpp"
#include "http/action.hpp"

using namespace http;
action_factory::action_factory(connection_creator &connection_creator_)
    : connection_creator_(connection_creator_) {}

std::unique_ptr<action> action_factory::create(const server &server_,
                                               const std::string &target,
                                               const fields &fields_,
                                               boost::beast::http::verb verb) {
  return std::make_unique<action>(connection_creator_, verb, target, server_,
                                  fields_);
}
