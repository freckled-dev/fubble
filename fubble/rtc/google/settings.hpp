#ifndef UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793
#define UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793

#include <istream>
#include <ostream>

namespace rtc {
namespace google {
struct settings {
  bool use_ip_v6{true};

  enum class audio_layer {
    default_,
    windows_core, // default on windows
    windows_core2,
    linux_alsa,
    linux_pulse, // default on linux
    android_java,
    android_open_sles,
    android_java_input_and_open_sles_output,
    android_aaudio,
    android_java_input_and_aaudio_output,
    dummy
  };
  audio_layer audio_layer_;
};

std::ostream &operator<<(std::ostream &out, settings::audio_layer print);
std::istream &operator>>(std::istream &in, settings::audio_layer &set);

} // namespace google
} // namespace rtc

#endif
