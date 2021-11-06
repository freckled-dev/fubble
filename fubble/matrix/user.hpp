#ifndef UUID_48B04D24_4E47_4D66_9D93_6D24AAC137EB
#define UUID_48B04D24_4E47_4D66_9D93_6D24AAC137EB

#include "logger.hpp"
#include <fubble/utils/signal.hpp>
#include <nlohmann/json_fwd.hpp>
#include <boost/optional.hpp>

namespace matrix {

enum class presence { online, unavailable, offline };

inline std::ostream &operator<<(std::ostream &out, const presence presence_) {
  switch (presence_) {
  case presence::online:
    return out << "online";
  case presence::offline:
    return out << "offline";
  case presence::unavailable:
    return out << "unavailable";
  }
  BOOST_ASSERT(false);
  return out;
}

class user {
public:
  user(const std::string &id);

  inline const std::string &get_id() const { return id; }

  inline presence get_presence() const { return presence_; }

  void set_display_name(const std::string &display_name_);
  inline const std::string &get_display_name() const { return display_name; }

  // TODO optimise. on_update gets called more often than neccessary
  utils::signal::signal<> on_update;

  void on_m_presence(const nlohmann::json &event);

protected:
  void set_presence(const presence &presence_parameter);
  void set_presence_from_string(const std::string &presence_parameter);

  matrix::logger logger;
  std::string id;
  presence presence_{presence::offline};
  std::string display_name;
};
} // namespace matrix

#endif
