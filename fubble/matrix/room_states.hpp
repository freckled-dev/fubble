#ifndef UUID_D676A18F_1B52_474C_970D_E000831069B9
#define UUID_D676A18F_1B52_474C_970D_E000831069B9

#include "events.hpp"
#include <boost/thread/future.hpp>
#include <memory>
#include <nlohmann/json.hpp>

namespace matrix {
class client;
class room_states {
public:
  struct custom {
    std::string key;
    std::string type;
    nlohmann::json data;
  };

  virtual ~room_states() = default;
  virtual boost::future<void> set_custom(const custom &set) = 0;
  virtual std::vector<custom> get_all_custom() const = 0;
  boost::signals2::signal<void(const custom &)> on_custom;

  virtual bool sync_event(const event::room_state_event &event) = 0;

  static std::unique_ptr<room_states> create(client &client_,
                                             const std::string &room_id);
};
} // namespace matrix

#endif
