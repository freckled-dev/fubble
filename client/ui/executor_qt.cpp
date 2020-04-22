#include "executor_qt.hpp"
#include "thread_helper.hpp"

using namespace client::ui;

executor_qt::executor_qt(QObject &post_to) : post_to(&post_to) {}

void executor_qt::submit(std::function<void()> &&call) {
  post_to_object(call, post_to);
}

void executor_qt::close() { BOOST_ASSERT(false); }

bool executor_qt::closed() {
  BOOST_ASSERT(false);
  return false;
}

bool executor_qt::try_executing_one() {
  BOOST_ASSERT(false);
  return false;
}
