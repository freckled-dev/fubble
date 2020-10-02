#include "timer.hpp"
#include <fmt/format.h>

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
  BOOST_ASSERT(!started);
  callback = callback_parameter;
  BOOST_ASSERT(callback);
  started = true;
  start_timer();
}

void interval_timer::stop() {
  BOOST_ASSERT(started);
  callback = {};
  timer.cancel();
  started = false;
}

void interval_timer::start_timer() {
  timer.expires_after(interval);
  std::weak_ptr<int> weak_alive_check = alive_check;
  timer.async_wait(
      [this, weak_alive_check = std::move(weak_alive_check)](auto error) {
        if (error) {
          BOOST_ASSERT(error == boost::asio::error::operation_aborted);
          return;
        }
        if (!weak_alive_check.lock())
          return;
        on_timeout();
      });
}

void interval_timer::on_timeout() {
  BOOST_ASSERT(callback);
  std::weak_ptr<int> weak_alive_check = alive_check;
  callback();
  // callback might have destroyed this instance. so ensure we are still alive
  if (!weak_alive_check.lock())
    return;
  if (!started)
    return;
  start_timer();
}
