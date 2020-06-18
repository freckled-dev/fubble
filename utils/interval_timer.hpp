#ifndef UUID_BDDB7AA2_2E88_4C01_BACE_FD2017816F91
#define UUID_BDDB7AA2_2E88_4C01_BACE_FD2017816F91

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

namespace utils {
class interval_timer {
public:
  interval_timer(boost::asio::io_context &context,
                 std::chrono::steady_clock::duration interval);

  using callack_type = std::function<void()>;
  void start(const callack_type &callback);
  void stop();

protected:
  void start_timer();
  void on_timeout(boost::system::error_code error);

  boost::asio::io_context &context;
  std::chrono::steady_clock::duration interval;
  boost::asio::steady_timer timer{context};
  callack_type callback;
};
} // namespace utils

#endif
