#include <sigslot/signal.hpp>

int main(int, char *[]) {
#if 1
  sigslot::signal<std::shared_ptr<int>> signal;
  sigslot::scoped_connection connection = signal.connect([](auto) {});
  signal(nullptr);
#endif
  return 0;
}
