#ifndef UUID_FFD30331_1D28_49CB_8229_27C2128854C2
#define UUID_FFD30331_1D28_49CB_8229_27C2128854C2

#include <string>

namespace fubble {
namespace v4l2_hw_h264 {
struct config {
  std::string path;
  int width{};
  int height{};
  int frame_rate{};
};
} // namespace v4l2_hw_h264
} // namespace fubble

#endif
