#ifndef UUID_9A081DDC_5E92_41A9_8E84_9339C03C7B20
#define UUID_9A081DDC_5E92_41A9_8E84_9339C03C7B20

#include <fubble/utils/export.hpp>
#include <functional>
#include <memory>

namespace fubble::lifetime {
// TODO this class is NOT threadsafe. set a thread id and verify it!
class FUBBLE_PUBLIC checker {
public:
  checker();
  ~checker();
  checker(const checker &) = delete;

  template <class... Args, class Func>
  std::function<void(Args...)> wrap(Func wrap_) {
    return [lifetime_checker = std::weak_ptr<int>(lifetime_checker),
            wrap_](Args... args) {
      if (!lifetime_checker.lock())
        return; // owning class died
      wrap_(args...);
    };
  }

protected:
  // pimpl/impl due to inclusion of <thread> header
  struct thread_checker;

  const std::shared_ptr<int> lifetime_checker = std::make_shared<int>();
  const std::unique_ptr<thread_checker> thread_checker_;
};
} // namespace fubble::lifetime

#endif
