#ifndef UUID_B11D00BB_A04E_447C_BB07_ABF86DE333A2
#define UUID_B11D00BB_A04E_447C_BB07_ABF86DE333A2

#include <fubble/utils/signal.hpp>

namespace client {
class room;
class rooms {
public:
  rooms();
  ~rooms();

  utils::signal::signal<> on_set;
  void set(const std::shared_ptr<room> &set_);
  std::shared_ptr<room> get() const;

protected:
  std::shared_ptr<room> room_;
};
} // namespace client

#endif
