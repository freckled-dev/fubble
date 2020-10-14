#ifndef UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7
#define UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7

#include "fields.hpp"
#include "logger.hpp"
#include <boost/beast/http/status.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json_fwd.hpp>

namespace http {
class action;
class action_factory;
class client {
public:
  client(const std::shared_ptr<action_factory> &action_factory_,
         const std::pair<server, fields> &server_and_fields);
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
  async_result_future do_action(std::unique_ptr<action> &&action_);
  void add_action(const std::shared_ptr<action> &action_);
  void remove_action(const action *action_);

  http::logger logger{"client"};
  std::shared_ptr<action_factory> action_factory_;
  boost::inline_executor executor;
  const server server_;
  const fields fields_;
  std::vector<std::shared_ptr<action>> actions;
};
} // namespace http

#endif
