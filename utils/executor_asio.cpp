#include "executor_asio.hpp"
#include <boost/asio/post.hpp>

executor_asio::executor_asio(boost::asio::executor &context)
    : context(context) {}

executor_asio::executor_asio(boost::asio::io_context &context)
    : context(context.get_executor()) {}

void executor_asio::submit(std::function<void()> &&call) {
  boost::asio::post(context, std::move(call));
}

void executor_asio::close() { BOOST_ASSERT_MSG(false, "must not reached"); }
bool executor_asio::closed() {
  BOOST_ASSERT_MSG(false, "must not reached");
  return false;
}
bool executor_asio::try_executing_one() {
  BOOST_ASSERT_MSG(false, "must not reached");
  return false;
}
