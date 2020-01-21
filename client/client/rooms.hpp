#ifndef UUID_B11D00BB_A04E_447C_BB07_ABF86DE333A2
#define UUID_B11D00BB_A04E_447C_BB07_ABF86DE333A2

#include <boost/signals2/signal.hpp>

namespace client {
class room;
class rooms {
public:
  rooms();
  ~rooms();

  boost::signals2::signal<void()> on_set;
  void set(const std::shared_ptr<room> &set_);

protected:
  std::shared_ptr<room> room_;
};
} // namespace client

#endif
