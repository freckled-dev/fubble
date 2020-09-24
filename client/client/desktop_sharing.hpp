#ifndef UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D
#define UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace rtc::google::capture::desktop {
class interval_capturer;
}

namespace client {
class desktop_sharing {
public:
  enum class type { screen, window };
  virtual void set(std::string id, type type_) = 0;
  virtual void reset() = 0;

  struct preview {
    std::unique_ptr<rtc::google::capture::desktop::interval_capturer> capturer;
  };
  virtual std::vector<preview> get_screen_previews() = 0;
  virtual std::vector<preview> get_window_previews() = 0;

  using interval_capturer_factory_type = std::function<
      std::unique_ptr<rtc::google::capture::desktop::interval_capturer>()>;
  static std::unique_ptr<desktop_sharing>
  create(interval_capturer_factory_type interval_capturer_factory);
};
} // namespace client

#endif
