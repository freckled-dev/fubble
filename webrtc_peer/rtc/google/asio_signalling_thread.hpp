#ifndef UUID_4D2DAB75_A9D1_4415_8754_D7BFE834C262
#define UUID_4D2DAB75_A9D1_4415_8754_D7BFE834C262

#include "rtc/logger.hpp"
#include <boost/asio/steady_timer.hpp>
#include <rtc_base/thread.h>
#include <thread>

namespace rtc {
namespace google {
// TODO refactor so it derives from `rtc::Thread` and must not get called in an
// interval
class asio_signalling_thread {
public:
  asio_signalling_thread(boost::asio::io_context &asio_);
  ~asio_signalling_thread();

  rtc::Thread &get_native() const;
  void stop();

protected:
  void trigger_wait();
  void on_waited(const boost::system::error_code &error);

  static constexpr int maximum_update_time_ms = 10;
  rtc::logger logger{"asio_signalling_thread"};
  boost::asio::io_context &asio;
  using timer_type = boost::asio::steady_timer;
  timer_type timer{asio};
  rtc::Thread *native;
  bool stopped{};
};
} // namespace google
} // namespace rtc

#endif
