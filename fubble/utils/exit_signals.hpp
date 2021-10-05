#ifndef UTILS_EXIT_SIGNALS_HPP
#define UTILS_EXIT_SIGNALS_HPP

#include "fubble/utils/logging/logger.hpp"
#include <boost/asio/io_context.hpp>

class exit_signals {
public:
  exit_signals(boost::asio::io_context &executor);
  ~exit_signals();

  using callback_type = std::function<void(const boost::system::error_code &)>;
  void async_wait(callback_type callback);
  void close();

private:
  logging::logger logger{"exit_signals"};
  struct implementation;
  std::unique_ptr<implementation> impl;
};

#endif
