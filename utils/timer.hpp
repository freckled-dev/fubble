#ifndef UUID_BDDB7AA2_2E88_4C01_BACE_FD2017816F91
#define UUID_BDDB7AA2_2E88_4C01_BACE_FD2017816F91

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

// TODO refactor to pimpl
namespace utils {
class one_shot_timer {
public:
  one_shot_timer(boost::asio::io_context &context,
                 std::chrono::steady_clock::duration timeout);
  using callack_type = std::function<void()>;
  void start(const callack_type &callback);
  void stop();

protected:
  void on_timeout(boost::system::error_code error);

  boost::asio::io_context &context;
  std::chrono::steady_clock::duration timeout;
  boost::asio::steady_timer timer{context};
  callack_type callback;
};

class interval_timer {
public:
  interval_timer(boost::asio::io_context &context,
                 std::chrono::steady_clock::duration interval);

  using callack_type = std::function<void()>;
  void start(const callack_type &callback);
  void stop();

protected:
  void start_timer();
  void on_timeout();

  boost::asio::io_context &context;
  std::chrono::steady_clock::duration interval;
  boost::asio::steady_timer timer{context};
  callack_type callback;
  bool started{};
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
};

class timer_factory {
public:
  timer_factory(boost::asio::io_context &context) : context(context) {}

  std::unique_ptr<one_shot_timer>
  create_one_shot_timer(const std::chrono::steady_clock::duration &duration) {
    return std::make_unique<one_shot_timer>(context, duration);
  }
  std::unique_ptr<interval_timer>
  create_interval_timer(const std::chrono::steady_clock::duration &duration) {
    return std::make_unique<interval_timer>(context, duration);
  }

protected:
  boost::asio::io_context &context;
}; // namespace utils
} // namespace utils

#endif
