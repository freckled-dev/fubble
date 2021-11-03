#ifndef UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793
#define UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793

#include <fubble/utils/export.hpp>
#include <istream>
#include <ostream>

namespace rtc {
namespace google {
// options taken from peer_connection_interface.h
struct FUBBLE_PUBLIC settings {
  bool use_ip_v6{true};
  bool disable_ipv6_on_wifi = false;

  struct audio {
    enum class layer {
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
    layer layer_{layer::default_};
    // TODO refactor to optional, and set no default values
    int jitter_buffer_max_packets = 200;
    bool jitter_buffer_fast_accelerate = false;
    int jitter_buffer_min_delay_ms = 0;
    bool jitter_buffer_enable_rtx_handling = false;

    bool enable_echo_canceller{true};
    bool enable_gain_controller{true};
    bool enable_high_pass_filter{true};
    bool enable_voice_detection{true};
    bool enable_noise_suppression{true};
    bool enable_typing_detection{true};
  };
  audio audio_;
};

FUBBLE_PUBLIC std::ostream &operator<<(std::ostream &out,
                                       settings::audio::layer print);
FUBBLE_PUBLIC std::istream &operator>>(std::istream &in,
                                       settings::audio::layer &set);

} // namespace google
} // namespace rtc

#endif
