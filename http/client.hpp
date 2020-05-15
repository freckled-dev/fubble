#ifndef UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7
#define UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7

#include "fields.hpp"
#include "logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json_fwd.hpp>

namespace http {
class action;
class client {
public:
  // TODO remove context, replace with action_factory
  client(boost::asio::io_context &context,
         const std::pair<server, fields> &server_and_fields);
  client(boost::asio::io_context &context, const server &server_,
         const fields &fields_);
  ~client();

  using async_result = std::pair<boost::beast::http::status, nlohmann::json>;
  using async_result_future = boost::future<async_result>;
  async_result_future get(const std::string &target);
  std::unique_ptr<action> get_action(const std::string &target);
  async_result_future post(const std::string &target,
                           const nlohmann::json &content);
  async_result_future put(const std::string &target,
                          const nlohmann::json &content);

protected:
  async_result_future do_action(std::shared_ptr<action> &action_);
  void add_action(const std::shared_ptr<action> &action_);
  void remove_action(const action *action_);

  http::logger logger{"client"};
  boost::asio::io_context &context;
  boost::inline_executor executor;
  const server server_;
  const fields fields_;
  std::vector<std::shared_ptr<action>> actions;
};
} // namespace http

#endif
