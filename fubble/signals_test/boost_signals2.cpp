#include <boost/signals2/signal.hpp>

int main(int, char *[]) {
#if 1
  boost::signals2::signal<void(std::shared_ptr<int>)> signal;
  boost::signals2::scoped_connection connection = signal.connect([](auto) {});
  signal(nullptr);
#endif
  return 0;
}
