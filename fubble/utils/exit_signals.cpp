#include "exit_signals.hpp"
#include <boost/asio/signal_set.hpp>
#include <fmt/format.h>

struct exit_signals::implementation {
  implementation(boost::asio::io_context &executor) : waiter{executor} {
    waiter.add(SIGINT);
    waiter.add(SIGTERM);
  }
  boost::asio::signal_set waiter;
};

exit_signals::exit_signals(boost::asio::io_context &executor)
    : impl{std::make_unique<implementation>(executor)} {}

exit_signals::~exit_signals() = default;

void exit_signals::async_wait(callback_type callback) {
  impl->waiter.async_wait([this, callback](const auto &error, auto signal) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << fmt::format("the signal '{}' occured, with the error:'{}'", signal,
                       error.message());
    callback(error);
  });
}

void exit_signals::close() { impl->waiter.cancel(); }
