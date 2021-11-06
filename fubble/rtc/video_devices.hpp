#ifndef UUID_F1B734FA_D783_43C6_9E78_FFB38C3015D5
#define UUID_F1B734FA_D783_43C6_9E78_FFB38C3015D5

#include <fubble/utils/signal.hpp>
#include <string>
#include <vector>

namespace utils {
class interval_timer;
}

namespace rtc {
class video_devices {
public:
  virtual ~video_devices() = default;
  struct information {
    std::string name;
    std::string id;
    bool operator==(const information &other) const {
      return other.name == name && other.id == id;
    }
    bool operator!=(const information &other) const {
      return !(other == *this);
    }
  };
  virtual bool enumerate() = 0;
  virtual std::vector<information> get_enumerated() const = 0;
  utils::signal::signal<> on_enumerated_changed;

  static std::unique_ptr<video_devices>
  create_interval_enumerating(std::shared_ptr<video_devices> adopt,
                              utils::interval_timer &timer);
};
class video_devices_noop : public video_devices {
  bool enumerate() override { return false; }
  std::vector<information> get_enumerated() const override { return {}; }
};
} // namespace rtc

#endif
