#ifndef UUID_F1B734FA_D783_43C6_9E78_FFB38C3015D5
#define UUID_F1B734FA_D783_43C6_9E78_FFB38C3015D5

#include <string>
#include <vector>

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
  virtual std::vector<information> get_enumerated() = 0;
};
class video_devices_noop : public video_devices {
  bool enumerate() override { return false; }
  std::vector<information> get_enumerated() override { return {}; }
};
} // namespace rtc

#endif
