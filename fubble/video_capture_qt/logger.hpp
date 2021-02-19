#ifndef UUID_1EFC2627_7E34_439F_AB3A_8569A1E3332A
#define UUID_1EFC2627_7E34_439F_AB3A_8569A1E3332A

#include <fubble/utils/logging/logger.hpp>

namespace video_capture_qt {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("video_capture_qt", descriptor) {}
};
} // namespace video_capture_qt

#endif
