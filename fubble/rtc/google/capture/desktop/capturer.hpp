#ifndef UUID_2A2B61CE_A845_4E31_ACD4_40E670F9B25D
#define UUID_2A2B61CE_A845_4E31_ACD4_40E670F9B25D

#include <boost/thread/future.hpp>
#include <cstdint>
#include <fubble/rtc/google/video_source.hpp>
#include <fubble/utils/export.hpp>
#include <memory>

namespace utils {
class interval_timer;
}

namespace rtc {
namespace google {
namespace capture {
namespace desktop {

class FUBBLE_PUBLIC capturer {
public:
  virtual ~capturer() = default;

  virtual boost::future<void> capture() = 0;
  virtual std::intptr_t get_id() = 0;
  virtual std::string get_title() = 0;

  virtual std::shared_ptr<video_source> get_source() const = 0;

  static std::unique_ptr<capturer> create(std::intptr_t id, bool cursor);
  static std::unique_ptr<capturer> create_without_cursor(std::intptr_t id);
  static std::unique_ptr<capturer> create_with_cursor(std::intptr_t id);
  static std::unique_ptr<capturer> create_screen(std::intptr_t id, bool cursor);
  static std::unique_ptr<capturer> create_window(std::intptr_t id, bool cursor);
};

class FUBBLE_PUBLIC interval_capturer {
public:
  virtual ~interval_capturer() = default;
  virtual boost::future<void> start() = 0;
  virtual void stop() = 0;
  virtual bool get_started() const = 0;
  virtual capturer &get_capturer() = 0;

  static std::unique_ptr<interval_capturer>
  create(std::unique_ptr<utils::interval_timer> timer,
         std::unique_ptr<capturer> delegate);
};

} // namespace desktop
} // namespace capture
} // namespace google
} // namespace rtc

#endif
