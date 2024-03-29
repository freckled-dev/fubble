#ifndef UUID_EA9C8A5A_DBAB_404C_B310_39FC7A74A99B
#define UUID_EA9C8A5A_DBAB_404C_B310_39FC7A74A99B

#include <fubble/rtc/track.hpp>

namespace rtc {
class video_track : public rtc::track {
public:
  virtual ~video_track() = default;

  enum class content_hint { none, fluid, detailed, text };
  virtual void set_content_hint(const content_hint hint) = 0;
};
} // namespace rtc

#endif
