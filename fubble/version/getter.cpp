#include "fubble/version/getter.hpp"
#include "fubble/version/logger.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <nlohmann/json.hpp>

using namespace version;

namespace {
class getter_impl final : public getter {
  version::logger logger{"getter_impl"};
  boost::inline_executor executor;
  const std::shared_ptr<http::client> http_client;

public:
  getter_impl(std::shared_ptr<http::client> http_client)
      : http_client{http_client} {}

  boost::future<result> get() override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "get";
    return http_client->get("/").then(
        executor, [this](auto result) { return got(result); });
  }

  result got(boost::future<http::client::async_result> &future) {
    auto got_result = future.get();
    auto json = got_result.second;
    BOOST_LOG_SEV(logger, logging::severity::info) << "got:" << json.dump();
    result return_;
    return_.current_version = json["current_version"];
    return_.minimum_version = json["minimum_version"];
    return return_;
  }
};
} // namespace

std::unique_ptr<getter>
getter::create(std::shared_ptr<http::client> http_client) {
  return std::make_unique<getter_impl>(http_client);
}
