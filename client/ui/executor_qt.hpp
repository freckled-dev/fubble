#ifndef UUID_ACE8E33B_E742_451A_B99C_1CB68BB1B7C9
#define UUID_ACE8E33B_E742_451A_B99C_1CB68BB1B7C9

#include <functional>

namespace client::ui {
class executor_qt {
public:
  void submit(std::function<void()> &&call);
  void close();
  bool closed();
  bool try_executing_one();
};
} // namespace client::ui

#endif
