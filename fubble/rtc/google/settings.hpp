#ifndef UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793
#define UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793

#include <istream>
#include <ostream>

namespace rtc {
namespace google {
// options taken from peer_connection_interface.h
struct settings {
  bool use_ip_v6{true};
  bool disable_ipv6_on_wifi = false;

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
  audio_layer audio_layer_{audio_layer::default_};
  // TODO refactor to optional, and set no default values
  int audio_jitter_buffer_max_packets = 200;
  bool audio_jitter_buffer_fast_accelerate = false;
  int audio_jitter_buffer_min_delay_ms = 0;
  bool audio_jitter_buffer_enable_rtx_handling = false;
};

std::ostream &operator<<(std::ostream &out, settings::audio_layer print);
std::istream &operator>>(std::istream &in, settings::audio_layer &set);

} // namespace google
} // namespace rtc

#endif
