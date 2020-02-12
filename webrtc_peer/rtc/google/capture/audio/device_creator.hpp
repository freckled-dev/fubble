#ifndef UUID_4B77AAD1_CB95_4352_BE36_BAED41D706DA
#define UUID_4B77AAD1_CB95_4352_BE36_BAED41D706DA

#include <memory>

namespace rtc::google {
class factory;
namespace capture::audio {
class device;
class device_creator {
public:
  device_creator(factory &factory_);

  std::unique_ptr<device> create();

protected:
  factory &factory_;
};
} // namespace capture::audio
} // namespace rtc::google

#endif
