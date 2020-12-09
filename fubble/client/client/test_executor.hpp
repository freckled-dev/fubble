#ifndef UUID_75951E51_62CF_4F19_898D_31BEAA0ADB56
#define UUID_75951E51_62CF_4F19_898D_31BEAA0ADB56

#include "fubble/rtc/google/asio_signaling_thread.hpp"
#include "fubble/utils/executor_asio.hpp"
#include "fubble/utils/logging/logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <boost/thread/executors/inline_executor.hpp>

struct test_executor {
  logging::logger logger{"Room"};
  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{
      context}; // TODO remove!
  boost::inline_executor inline_executor;
  rtc::google::asio_signaling_thread rtc_signaling_thread{context};

  void run_context() {
    context.run();
    context.reset();
  }
};

#endif
