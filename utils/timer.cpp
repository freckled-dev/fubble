#include "timer.hpp"

using namespace utils;

one_shot_timer::one_shot_timer(boost::asio::io_context &context,
                               std::chrono::steady_clock::duration timeout)
    : context(context), timeout{timeout} {}

void one_shot_timer::start(const callack_type &callback_parameter) {
  BOOST_ASSERT(!callback);
  callback = callback_parameter;
  BOOST_ASSERT(callback);
  timer.expires_after(timeout);
  timer.async_wait([this](auto error) { on_timeout(error); });
}

void one_shot_timer::stop() { timer.cancel(); }

void one_shot_timer::on_timeout(boost::system::error_code error) {
  if (error) {
    BOOST_ASSERT(error == boost::asio::error::operation_aborted);
    return;
  }
  BOOST_ASSERT(callback);
  callback();
  callback = callack_type{};
}

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
