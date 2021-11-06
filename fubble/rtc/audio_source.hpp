#ifndef UUID_15E511A1_D1CD_40FF_A4A6_4D46EBAE7EFF
#define UUID_15E511A1_D1CD_40FF_A4A6_4D46EBAE7EFF

#include <fubble/utils/signal.hpp>
#include <fubble/rtc/audio_data.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
class FUBBLE_PUBLIC audio_source {
public:
  virtual ~audio_source() = default;
  utils::signal::signal<const audio_data &> on_data;
};
} // namespace rtc

#endif
