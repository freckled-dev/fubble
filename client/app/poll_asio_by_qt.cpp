#include "poll_asio_by_qt.hpp"

using namespace client;

poll_asio_by_qt::poll_asio_by_qt(boost::asio::io_context &context)
    : context(context) {
  connect(&timer, &QTimer::timeout, this, &poll_asio_by_qt::on_timeout);
}

void poll_asio_by_qt::run() {
  static const auto timeout = std::chrono::milliseconds(5);
  timer.start(timeout);
}

void poll_asio_by_qt::on_timeout() { context.poll(); }
