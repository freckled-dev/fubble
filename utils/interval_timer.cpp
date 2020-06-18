#include "interval_timer.hpp"

using namespace utils;

interval_timer::interval_timer(boost::asio::io_context &context,
                               std::chrono::steady_clock::duration interval)
    : context(context), interval{interval} {}

void interval_timer::start(const callack_type &callback_parameter) {
  BOOST_ASSERT(!callback);
  callback = callback_parameter;
  BOOST_ASSERT(callback);
  start_timer();
}

void interval_timer::stop() { timer.cancel(); }

void interval_timer::start_timer() {
  timer.expires_after(interval);
  timer.async_wait([this](auto error) { on_timeout(error); });
}

void interval_timer::on_timeout(boost::system::error_code error) {
  if (error) {
    BOOST_ASSERT(error == boost::asio::error::operation_aborted);
    return;
  }
  BOOST_ASSERT(callback);
  callback();
  start_timer();
}

