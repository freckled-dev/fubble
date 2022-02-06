#include "lifetime.hpp"
#include <boost/assert.hpp>
#include <thread>

using namespace fubble::lifetime;

checker::checker() : thread_checker_{std::make_unique<thread_checker>()} {}

checker::~checker() = default;

struct checker::thread_checker {
  thread_checker() : id{std::this_thread::get_id()} {}

  void assert_thread() { BOOST_ASSERT(id == std::this_thread::get_id()); }

  const std::thread::id id;
};
