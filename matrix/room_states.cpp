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
    return http_client->put(target, set.data).then(executor, [](auto result) {
      return error::check_matrix_response(result);
    });
  }

  std::vector<custom> get_all_custom() const override { return customs; }

  bool sync_event(const event::room_state_event &event) override {
    const std::string &state_key = event.state_key;
    if (boost::starts_with(state_key, "m."))
      return false;

    return true;
  }

protected:
  boost::inline_executor executor;
  client &client_;
  const std::string room_id;
  std::unique_ptr<http::client> http_client;
  std::vector<custom> customs;
};
} // namespace

std::unique_ptr<room_states> room_states::create(client &client_,
                                                 const std::string &room_id) {
  return std::make_unique<room_states_impl>(client_, room_id);
}
