#include "asio_signalling_thread.hpp"
#include "rtc_base/null_socket_server.h"
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <rtc_base/null_socket_server.h>
#include <rtc_base/time_utils.h>

using namespace rtc::google;

asio_signalling_thread::asio_signalling_thread(boost::asio::io_context &asio_)
    : asio(asio_) {
  native = ThreadManager::Instance()->WrapCurrentThread();
  BOOST_ASSERT(native);
  trigger_wait();
}

asio_signalling_thread::~asio_signalling_thread() {}

rtc::Thread &asio_signalling_thread::get_native() const { return *native; }

void asio_signalling_thread::trigger_wait() {
  timer.expires_after(std::chrono::milliseconds(50));
  timer.async_wait([this](auto error) { on_waited(error); });
}

void asio_signalling_thread::on_waited(const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted)
    return;
  BOOST_ASSERT(!error);
  trigger_wait();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "before ProcessMessages";
  native->ProcessMessages(10);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "after ProcessMessages";
}
