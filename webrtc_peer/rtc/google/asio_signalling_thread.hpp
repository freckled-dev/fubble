#ifndef UUID_4D2DAB75_A9D1_4415_8754_D7BFE834C262
#define UUID_4D2DAB75_A9D1_4415_8754_D7BFE834C262

#include "logging/logger.hpp"
#include <boost/asio/steady_timer.hpp>
#include <rtc_base/thread.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

namespace rtc::google {
class asio_signalling_thread : public rtc::Thread {
public:
  asio_signalling_thread(boost::asio::io_context &asio_);
  ~asio_signalling_thread() override;

  void Send(const Location &posted_from, MessageHandler *phandler,
            uint32_t id = 0, MessageData *pdata = nullptr) override {
    BOOST_ASSERT(false && "not implementated");
  }
  bool Get(Message *pmsg, int cmsWait = kForever,
           bool process_io = true) override {
    BOOST_ASSERT(false && "not implementated");
  }

  bool Peek(Message *pmsg, int cmsWait = 0) override {
    BOOST_ASSERT(false && "not implementated");
  }

  Message create_message(const Location &posted_from, MessageHandler *phandler,
                         uint32_t id = 0, MessageData *pdata = nullptr,
                         bool time_sensitive = false);

  void Post(const Location &posted_from, MessageHandler *phandler,
            uint32_t id = 0, MessageData *pdata = nullptr,
            bool time_sensitive = false) override;
  void PostDelayed(const Location &posted_from, int cmsDelay,
                   MessageHandler *phandler, uint32_t id = 0,
                   MessageData *pdata = nullptr) override;
  void PostAt(const Location &posted_from, int64_t tstamp,
              MessageHandler *phandler, uint32_t id = 0,
              MessageData *pdata = nullptr) override {
    BOOST_ASSERT(false && "not implementated");
  }

  void PostAt(const Location &posted_from, uint32_t tstamp,
              MessageHandler *phandler, uint32_t id = 0,
              MessageData *pdata = nullptr) override {
    BOOST_ASSERT(false && "not implementated");
  }

#if 0
  void Clear(MessageHandler *phandler, uint32_t id = MQID_ANY,
             MessageList *removed = nullptr) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "Clear, phandler:" << phandler;
    // BOOST_ASSERT(false && "not implementated");
  }
#endif

  void Dispatch(Message *pmsg) override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "Dispatch";
    Thread::Dispatch(pmsg);
  }

  void ReceiveSends() override { BOOST_ASSERT(false && "not implementated"); }

  int GetDelay() override { BOOST_ASSERT(false && "not implementated"); }

protected:
  logging::logger logger;
  boost::asio::io_context &asio;
  using timer_type = boost::asio::steady_timer;
  std::deque<std::unique_ptr<timer_type>> timers;
};
} // namespace rtc::google

#pragma clang diagnostic pop

#endif
