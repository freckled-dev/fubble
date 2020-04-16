#include "connection.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

connection::connection(boost::asio::io_context &context) : stream(context) {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "websocket::~connection()";
  if (!reading)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "read_promise is not fullfilled!";
  read_promise.set_exception(
      boost::system::system_error(boost::asio::error::operation_aborted));
}

connection::stream_type &connection::get_native() { return stream; }

static void completion_error(const boost::system::error_code &error) {
  if (!error)
    return;
  throw boost::system::system_error(error);
}

boost::future<void> connection::send(const std::string &message) {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "sending message: '" << message << "'";
#endif
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
  std::weak_ptr<int> weak_alive_check = alive_check;
  stream.async_write(
      boost::asio::buffer(item.message),
      [this, weak_alive_check](const auto &error, auto transferred) {
        if (!weak_alive_check.lock())
          return;
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
      << "closing websocket connection, this:" << this;
  boost::packaged_task<void(boost::system::error_code)> task(completion_error);
  auto result = task.get_future();
  stream.async_close(boost::beast::websocket::close_code::normal,
                     std::move(task));
  return result;
}

boost::future<std::string> connection::read() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "reading websocket connection, this:" << this;
  BOOST_ASSERT(!reading);
  reading = true;
  read_promise = boost::promise<std::string>();
  std::weak_ptr<int> weak_alive_check = alive_check;
  stream.async_read(buffer, [this, weak_alive_check](const auto &error,
                                                     auto bytes_transferred) {
    if (!weak_alive_check.lock())
      return;
    on_read(error, bytes_transferred);
  });
  return read_promise.get_future();
}

void connection::on_read(const boost::system::error_code &error, std::size_t) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "stream.async_read, this:" << this << ", error:" << error.message();
  if (error) {
    reading = false;
    read_promise.set_exception(boost::system::system_error(error));
    return;
  }
  std::string result = boost::beast::buffers_to_string(buffer.data());
  buffer.consume(buffer.size());
  read_promise.set_value(result);
  reading = false;
}
