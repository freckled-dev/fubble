#include "connection.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

connection::connection(boost::asio::io_context &context)
    : context(context), stream(context) {}

connection::stream_type &connection::get_native() { return stream; }

static void completion_error(const boost::system::error_code &error) {
  if (!error)
    return;
  throw boost::system::system_error(error);
}

boost::future<void> connection::send(const std::string &message) {
  send_queue.emplace();
  send_item &item = send_queue.back();
  item.message = message;
  if (!sending)
    send_next_from_queue();
  auto result = item.completion.get_future();
  return result;
}

void connection::send_next_from_queue() {
  sending = true;
  auto &item = send_queue.front();
  stream.async_write(boost::asio::buffer(item.message),
                     [this](const auto &error, auto transferred) {
                       on_send(error, transferred);
                     });
}

void connection::on_send(const boost::system::error_code &error, std::size_t) {
  auto &did_send = send_queue.front();
  if (error) {
    did_send.completion.set_exception(boost::system::system_error(error));
    return;
  }
  did_send.completion.set_value();
  send_queue.pop();
  if (send_queue.empty()) {
    BOOST_ASSERT(sending);
    sending = false;
    return;
  }
  send_next_from_queue();
}

boost::future<void> connection::close() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "closing websocket connection";
  boost::packaged_task<void(boost::system::error_code)> task(completion_error);
  auto result = task.get_future();
  stream.async_close(boost::beast::websocket::close_code::normal,
                     std::move(task));
  return result;
}

boost::future<std::string> connection::read() {
  boost::promise<std::string> promise;
  auto result = promise.get_future();
  stream.async_read(buffer, [this, promise = std::move(promise)](
                                const auto &error, auto) mutable {
    if (error) {
      promise.set_exception(boost::system::system_error(error));
      return;
    }
    std::string result = boost::beast::buffers_to_string(buffer.data());
    buffer.consume(buffer.size());
    promise.set_value(result);
  });
  return result;
}
