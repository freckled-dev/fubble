#ifndef EXECUTOR_ASIO_HPP
#define EXECUTOR_ASIO_HPP

#include <boost/asio/executor.hpp>
#include <boost/asio/io_context.hpp>
#include <functional>

class executor_asio {
public:
  executor_asio(boost::asio::executor &context);
  executor_asio(boost::asio::io_context &context);
  void submit(std::function<void()> &&call);
  void close();
  bool closed();
  bool try_executing_one();

private:
  boost::asio::executor context;
};

#endif
