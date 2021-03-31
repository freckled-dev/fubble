#ifndef UUID_71816140_2FD6_4EB5_8BE0_2D30A46EEAAA
#define UUID_71816140_2FD6_4EB5_8BE0_2D30A46EEAAA

#include <fubble/utils/export.hpp>
#include <ostream>

namespace rtc {
namespace video {
struct FUBBLE_PUBLIC capability {
  int width{1280};
  int height{720};
  int fps{30};
};
inline std::ostream &operator<<(std::ostream &out, const capability &print) {
  return out << "{width: " << print.width << ", height: " << print.height
             << ", fps: " << print.fps << "}";
}
} // namespace video
} // namespace rtc

#endif
