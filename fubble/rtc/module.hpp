#ifndef UUID_35C0E76A_8CA4_4636_8CAA_FF1FB40B1B74
#define UUID_35C0E76A_8CA4_4636_8CAA_FF1FB40B1B74

#include <fubble/rtc/factory.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
class FUBBLE_PUBLIC module {
public:
  virtual ~module() = default;
  virtual std::shared_ptr<factory> get_factory() = 0;
};
} // namespace rtc

#endif
