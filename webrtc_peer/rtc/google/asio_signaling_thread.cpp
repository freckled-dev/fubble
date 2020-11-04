#include "asio_signaling_thread.hpp"
#include "rtc_base/null_socket_server.h"
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <rtc_base/null_socket_server.h>
#include <rtc_base/time_utils.h>

using namespace rtc::google;

asio_signaling_thread::asio_signaling_thread(boost::asio::io_context &asio_)
    : asio(asio_) {
  native = ThreadManager::Instance()->WrapCurrentThread();
  BOOST_ASSERT(native);
  trigger_wait();
}

asio_signaling_thread::~asio_signaling_thread() {}

rtc::Thread &asio_signaling_thread::get_native() const { return *native; }

void asio_signaling_thread::trigger_wait() {
  if (stopped)
    return;
  static constexpr std::chrono::steady_clock::duration interval =
      std::chrono::milliseconds(50);
  timer.expires_after(interval);
  timer.async_wait([this](auto error) { on_waited(error); });
}

void asio_signaling_thread::on_waited(const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted)
    return;
  BOOST_ASSERT(!error);
  trigger_wait();
  native->ProcessMessages(maximum_update_time_ms);
}

void asio_signaling_thread::stop() { stopped = true; }
