#ifndef UUID_FFD30331_1D28_49CB_8229_27C2128854C2
#define UUID_FFD30331_1D28_49CB_8229_27C2128854C2

#include <fubble/utils/export.hpp>
#include <string>

namespace fubble {
namespace v4l2_hw_h264 {
struct FUBBLE_PUBLIC config {
  std::string path{"/dev/video0"};
  int width{1280};
  int height{720};
  int framerate{30};
};
} // namespace v4l2_hw_h264
} // namespace fubble

#endif
