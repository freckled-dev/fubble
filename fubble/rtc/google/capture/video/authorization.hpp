#ifndef UUID_88AC125A_153E_4FA5_BF32_4C04DC79772A
#define UUID_88AC125A_153E_4FA5_BF32_4C04DC79772A

#include <functional>
#include <memory>

namespace rtc {
namespace google {
namespace capture {
namespace video {
class authorization {
public:
  virtual ~authorization() = default;

  enum struct state { authorized, not_determined, denied, restricted };

  virtual state get_state() = 0;
  using on_finished_type = std::function<void(bool)>;
  virtual void request(on_finished_type on_finished) = 0;

  static std::unique_ptr<authorization> create();
};
} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
