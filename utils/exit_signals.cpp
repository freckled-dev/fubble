#include "exit_signals.hpp"
#include <boost/asio/signal_set.hpp>
#include <fmt/format.h>

struct exit_signals::implementation {
  implementation(boost::asio::executor &executor)
      : waiter{executor, SIGINT, SIGTERM} {}
  boost::asio::signal_set waiter;
};

exit_signals::exit_signals(boost::asio::executor &executor)
    : impl{std::make_unique<implementation>(executor)} {}

exit_signals::~exit_signals() = default;

void exit_signals::async_wait(callback_type callback) {
  impl->waiter.async_wait([this, callback](const auto &error, auto signal) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("the signal '{}' occured, with the error:'{}'", signal,
                       error.message());
    callback(error);
  });
}
