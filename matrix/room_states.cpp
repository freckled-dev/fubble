#include "room_states.hpp"
#include "client.hpp"
#include "error.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <fmt/format.h>

using namespace matrix;

namespace {
class room_states_impl : public room_states {
public:
  room_states_impl(client &client_, const std::string &room_id)
      : client_(client_), room_id(room_id) {
    http_client = client_.create_http_client();
  }

  boost::future<void> set_custom(const custom &set) override {
    auto target =
        fmt::format("rooms/{}/state/{}/{}", room_id, set.type, set.key);
    BOOST_ASSERT(!set.type.empty());
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << "setting custom state, target:" << target
        << ", data:" << set.data.dump();
    return http_client->put(target, set.data).then(executor, [](auto result) {
      return error::check_matrix_response(result);
    });
  }

  std::vector<custom> get_all_custom() const override {
    std::vector<custom> result;
    std::transform(customs.cbegin(), customs.cend(), std::back_inserter(result),
                   [](const auto &cast) { return cast.data; });
    return result;
  }

  bool sync_event(const event::room_state_event &event) override {
    auto custom_event_content =
        dynamic_cast<event::room::custom_state *>(event.content_.get());
    if (!custom_event_content)
      return false;
    BOOST_LOG_SEV(logger, logging::severity::debug) << "got a custom_state";
    custom_container add_or_set;
    add_or_set.data.data = custom_event_content->data;
    add_or_set.data.key = event.state_key;
    add_or_set.data.type = event.type;
    add_or_set.timestamp = event.origin_server_ts;
    auto found =
        std::find_if(customs.begin(), customs.end(), [&](const auto &check) {
          return check.data.type == event.type &&
                 check.data.key == event.state_key;
        });
    if (found == customs.end()) {
      customs.push_back(add_or_set);
      on_custom(add_or_set.data);
    } else if (found->timestamp < event.origin_server_ts) {
      *found = add_or_set;
      on_custom(add_or_set.data);
    } else {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << "ignoring custom event, due to a newer state being available";
    }
    return true;
  }

protected:
  boost::inline_executor executor;
  matrix::logger logger{"room_states_impl"};
  client &client_;
  const std::string room_id;
  std::unique_ptr<http::client> http_client;
  struct custom_container {
    custom data;
    std::chrono::system_clock::time_point timestamp;
  };
  std::vector<custom_container> customs;
};
} // namespace

std::unique_ptr<room_states> room_states::create(client &client_,
                                                 const std::string &room_id) {
  return std::make_unique<room_states_impl>(client_, room_id);
}
