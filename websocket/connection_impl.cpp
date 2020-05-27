#include "connection_impl.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

namespace {
connection_impl::stream_type make_stream(boost::asio::io_context &context,
                                         boost::asio::ssl::context &ssl_context,
                                         bool secure) {
  if (secure)
    return connection_impl::https_stream_type(context, ssl_context);
  return connection_impl::http_stream_type(context);
}
} // namespace

connection_impl::connection_impl(boost::asio::io_context &context, bool secure)
    : ssl_context{boost::asio::ssl::context::sslv23},
      stream(make_stream(context, ssl_context, secure)) {}

connection_impl::~connection_impl() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "websocket::~connection()";
  if (!reading)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "read_promise is not fullfilled!";
  read_promise.set_exception(
      boost::system::system_error(boost::asio::error::operation_aborted));
}

connection_impl::stream_type &connection_impl::get_native() { return stream; }

static void completion_error(const boost::system::error_code &error) {
  if (!error)
    return;
  throw boost::system::system_error(error);
}

boost::future<void> connection_impl::send(const std::string &message) {
#if 1
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

boost::asio::ssl::context &connection_impl::get_ssl_context() {
  return ssl_context;
}

void connection_impl::send_next_from_queue() {
  sending = true;
  auto &item = send_queue.front();
  std::weak_ptr<int> weak_alive_check = alive_check;
  std::visit(
      [&](auto &stream_) {
        stream_.async_write(
            boost::asio::buffer(item.message),
            [this, weak_alive_check](const auto &error, auto transferred) {
              if (!weak_alive_check.lock())
                return;
              on_send(error, transferred);
            });
      },
      stream);
}

void connection_impl::on_send(const boost::system::error_code &error,
                              std::size_t) {
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

boost::future<void> connection_impl::close() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "closing websocket connection, this:" << this;
  boost::packaged_task<void(boost::system::error_code)> task(completion_error);
  auto result = task.get_future();
  std::visit(
      [&](auto &stream_) {
        stream_.async_close(boost::beast::websocket::close_code::normal,
                            std::move(task));
      },
      stream);
  return result;
}

boost::future<std::string> connection_impl::read() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "reading websocket connection, this:" << this;
  BOOST_ASSERT(!reading);
  reading = true;
  read_promise = boost::promise<std::string>();
  std::weak_ptr<int> weak_alive_check = alive_check;
  std::visit(
      [&](auto &stream_) {
        stream_.async_read(buffer,
                           [this, weak_alive_check](const auto &error,
                                                    auto bytes_transferred) {
                             if (!weak_alive_check.lock())
                               return;
                             on_read(error, bytes_transferred);
                           });
      },
      stream);
  return read_promise.get_future();
}

void connection_impl::on_read(const boost::system::error_code &error,
                              std::size_t) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
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
