#include "asio_signalling_thread.hpp"
#include "rtc_base/null_socket_server.h"
#include <boost/asio/post.hpp>
#include <rtc_base/null_socket_server.h>
#include <rtc_base/time_utils.h>

using namespace rtc::google;

asio_signalling_thread::asio_signalling_thread(boost::asio::io_context &asio_)
    : rtc::Thread(std::unique_ptr<SocketServer>(new rtc::NullSocketServer())),
      asio(asio_) {
  boost::asio::post(
      asio, [this] { ThreadManager::Instance()->SetCurrentThread(this); });
}

asio_signalling_thread::~asio_signalling_thread() { Stop(); }

rtc::Message asio_signalling_thread::create_message(const Location &posted_from,
                                                    MessageHandler *phandler,
                                                    uint32_t id,
                                                    MessageData *pdata,
                                                    bool time_sensitive) {
  Message result;
  result.posted_from = posted_from;
  result.phandler = phandler;
  result.message_id = id;
  BOOST_ASSERT(MQID_DISPOSE != result.message_id);
  result.pdata = pdata;
  if (time_sensitive) {
    BOOST_ASSERT(false && "not implementated");
    const int kMaxMsgLatency = 150; // 150 ms
    result.ts_sensitive = rtc::TimeMillis() + kMaxMsgLatency;
  }
  return result;
}

void asio_signalling_thread::Post(const Location &posted_from,
                                  MessageHandler *phandler, uint32_t id,
                                  MessageData *pdata, bool time_sensitive) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "Post, phandler:" << phandler;
  if (IsQuitting()) {
    delete pdata;
    return;
  }
  auto message =
      create_message(posted_from, phandler, id, pdata, time_sensitive);
  boost::asio::post(asio, [this, message]() mutable { Dispatch(&message); });
}

void asio_signalling_thread::PostDelayed(const Location &posted_from,
                                         int cmsDelay, MessageHandler *phandler,
                                         uint32_t id, MessageData *pdata) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "PostDelayed, phandler:" << phandler << ", cmsDelay:" << cmsDelay;
  auto message = create_message(posted_from, phandler, id, pdata, false);
  auto timer = std::make_unique<timer_type>(asio);
  // TODO timer management
  timer->expires_from_now(std::chrono::milliseconds(cmsDelay));
  timer->async_wait([this, message](auto error) mutable {
    if (error == boost::asio::error::operation_aborted)
      return;
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "timer timed out, error:" << error.message();
    Dispatch(&message);
  });
  timers.emplace_back(std::move(timer));
}

#if 0
void asio_signalling_thread::Send(const Location &posted_from,
                                  MessageHandler *phandler, uint32_t id,
                                  MessageData *pdata) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "sending something to";
  rtc::Thread::Send(posted_from, phandler, id, pdata);
}
#endif
