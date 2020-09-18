#ifndef UUID_2A2B61CE_A845_4E31_ACD4_40E670F9B25D
#define UUID_2A2B61CE_A845_4E31_ACD4_40E670F9B25D

#include <boost/thread/future.hpp>
#include <cstdint>
#include <memory>

namespace rtc {
namespace google {
namespace capture {
namespace desktop {
class capturer {
public:
  virtual ~capturer() = default;

  virtual boost::future<void> start() = 0;
  virtual void stop() = 0;

  static std::unique_ptr<capturer> create_screen(std::intptr_t id);
  static std::unique_ptr<capturer> create_window(std::intptr_t id);
};
} // namespace desktop
} // namespace capture
} // namespace google
} // namespace rtc

#endif
