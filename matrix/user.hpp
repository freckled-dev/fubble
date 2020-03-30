#ifndef UUID_48B04D24_4E47_4D66_9D93_6D24AAC137EB
#define UUID_48B04D24_4E47_4D66_9D93_6D24AAC137EB

#include <boost/signals2/signal.hpp>

namespace matrix {

class user {
  std::string id;
  std::string presence;
  std::string display_name;

public:
  user(const std::string &id);

  inline const std::string &get_id() const { return id; }

  inline void set_presence(const std::string &presence_) {
    if (presence == presence_)
      return;
    presence = presence_;
    on_update();
  }
  inline const std::string &get_presence() const { return presence; }

  inline void set_display_name(const std::string &display_name_) {
    if (display_name == display_name_)
      return;
    display_name = display_name_;
    on_update();
  }
  inline const std::string &get_display_name() const { return display_name; }

  // TODO optimise. on_update gets called more often than neccessary
  boost::signals2::signal<void()> on_update;
};
} // namespace matrix

#endif
