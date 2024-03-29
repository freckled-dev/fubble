#ifndef UUID_F6B4996F_8793_4480_BBA6_A14657BA6797
#define UUID_F6B4996F_8793_4480_BBA6_A14657BA6797

#include <fubble/utils/signal.hpp>
#include <memory>
#include <vector>

namespace client {
class peer;
class peers {
public:
  using peer_ptr = std::shared_ptr<peer>;

  ~peers();
  void add(const peer_ptr &add_);
  std::vector<peer_ptr> get_all();
  void remove(const peer_ptr &remove_);

  utils::signal::signal<peer_ptr> on_added;
  utils::signal::signal<peer_ptr> on_removed;

protected:
  std::vector<peer_ptr> container;
};
} // namespace client

#endif
