#ifndef UUID_CD1D441F_1E79_4250_811E_F72162D8F634
#define UUID_CD1D441F_1E79_4250_811E_F72162D8F634

#include <cstdint>
#include <fubble/utils/export.hpp>
#include <memory>
#include <string>
#include <vector>

namespace rtc {
namespace google {
namespace capture {
namespace desktop {
class FUBBLE_PUBLIC enumerator {
public:
  struct information {
    std::intptr_t id;
    std::string title;
  };
  virtual ~enumerator() = default;
  virtual void enumerate() = 0;

  virtual std::vector<information> get_screens() const = 0;
  virtual std::vector<information> get_windows() const = 0;

  static std::unique_ptr<enumerator> create();
};
} // namespace desktop
} // namespace capture
} // namespace google
} // namespace rtc

#endif
