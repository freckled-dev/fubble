#ifndef UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793
#define UUID_313BBDF4_E8A1_43F8_B822_CAB9749D6793

namespace rtc {
namespace google {
struct settings {
  bool use_ip_v6{true};
#if BOOST_OS_WINDOWS
  bool windows_use_core_audio2{false}; // windows core audio 2 is experimental
#endif
};
} // namespace google
} // namespace rtc

#endif
