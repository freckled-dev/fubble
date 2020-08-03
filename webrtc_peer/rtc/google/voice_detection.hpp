#ifndef UUID_EE94E3D7_CC32_4FBF_A51C_8871D7D52AD0
#define UUID_EE94E3D7_CC32_4FBF_A51C_8871D7D52AD0

#include <memory>

namespace rtc::google {
class audio_data;
class voice_detection {
public:
  virtual ~voice_detection() = default;

  virtual bool detect(const audio_data &data) = 0;

  static std::unique_ptr<voice_detection> create();
};
} // namespace rtc::google

#endif
