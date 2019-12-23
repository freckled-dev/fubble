#include "peers.hpp"
#include <algorithm>
#include <boost/assert.hpp>

using namespace client;

peers::~peers() = default;

void peers::add(const peer_ptr &add_) { container.push_back(add_); }

std::vector<peers::peer_ptr> peers::get_all() { return container; }

void peers::remove(const peer_ptr &remove_) {
  auto found = std::find(container.cbegin(), container.cend(), remove_);
  BOOST_ASSERT(found != container.cend());
}
