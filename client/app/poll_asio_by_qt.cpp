#include "poll_asio_by_qt.hpp"

using namespace client;

poll_asio_by_qt::poll_asio_by_qt(boost::asio::io_context &context)
    : context(context) {
  connect(&timer, SIGNAL(timeout()), this, SLOT(on_timeout));
}

void poll_asio_by_qt::run() { timer.start(std::chrono::milliseconds(50)); }

void poll_asio_by_qt::on_timeout() { context.poll(); }
