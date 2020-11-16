#include "connection_impl.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

namespace {
std::shared_ptr<connection_impl::stream_type>
make_stream(boost::asio::io_context &context,
            boost::asio::ssl::context &ssl_context, bool secure) {
  if (secure)
    return std::make_shared<connection_impl::stream_type>(
        connection_impl::https_stream_type(context, ssl_context));
  return std::make_shared<connection_impl::stream_type>(
      connection_impl::http_stream_type(context));
}
} // namespace

connection_impl::connection_impl(boost::asio::io_context &context, bool secure)
    : ssl_context{boost::asio::ssl::context::sslv23},
      stream(make_stream(context, ssl_context, secure)) {
  // stream and buffer are shared, because they all have to stay valid as long
  // an async operation is active
  // https://www.boost.org/doc/libs/1_73_0/libs/beast/doc/html/beast/ref/boost__beast__websocket__stream/_stream.html
}

connection_impl::~connection_impl() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "websocket::~connection()";
  if (!reading)
    return;
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "read_promise is not fullfilled!";
  read_promise->set_exception(
      boost::system::system_error(boost::asio::error::operation_aborted));
}

connection_impl::stream_type &connection_impl::get_native() { return *stream; }

namespace {
void completion_error(const boost::system::error_code &error) {
  if (!error)
    return;
  BOOST_THROW_EXCEPTION(boost::system::system_error(error));
}
} // namespace

boost::future<void> connection_impl::send(const std::string &message) {
#if 1
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "sending message: '" << message << "'";
#endif
  send_queue->emplace();
  send_item &item = send_queue->back();
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
  auto &item = send_queue->front();
  std::weak_ptr<int> weak_alive_check = alive_check;
  boost::apply_visitor(
      [&, this](auto &stream_) {
        stream_.async_write(
            boost::asio::buffer(item.message),
            [this, weak_alive_check, send_queue = send_queue,
             stream = stream](const auto &error, auto transferred) {
              if (!weak_alive_check.lock())
                return;
              on_send(error, transferred);
            });
      },
      *stream);
}

void connection_impl::on_send(const boost::system::error_code &error,
                              std::size_t) {
  auto &did_send = send_queue->front();
  if (error) {
    did_send.completion.set_exception(boost::system::system_error(error));
    return;
  }
  did_send.completion.set_value();
  send_queue->pop();
  if (send_queue->empty()) {
    BOOST_ASSERT(sending);
    sending = false;
    return;
  }
  send_next_from_queue();
}

boost::future<void> connection_impl::close() {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "closing websocket connection, this:" << this;
  boost::packaged_task<void(boost::system::error_code)> task(
      [stream = stream, this](auto result) {
        BOOST_LOG_SEV(this->logger, logging::severity::debug)
            << "async_close handler";
        completion_error(result);
      });
  auto result = task.get_future();
  boost::apply_visitor(
      [&](auto &stream_) {
        stream_.async_close(boost::beast::websocket::close_code::normal,
                            std::move(task));
      },
      *stream);
  return result;
}

boost::future<std::string> connection_impl::read() {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "reading websocket connection, this:" << this;
  BOOST_ASSERT(!reading);
  BOOST_ASSERT(!read_promise);
  reading = true;
  read_promise = std::make_shared<boost::promise<std::string>>();
  std::weak_ptr<int> weak_alive_check = alive_check;
  boost::apply_visitor(
      [&, this](auto &stream_) {
        stream_.async_read(
            *read_buffer,
            [this, weak_alive_check, read_buffer = read_buffer,
             stream = stream](const auto &error, auto bytes_transferred) {
              if (!weak_alive_check.lock())
                return;
              on_read(error, bytes_transferred);
            });
      },
      *stream);
  return read_promise->get_future();
}

void connection_impl::on_read(const boost::system::error_code &error,
                              std::size_t) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "stream.async_read, this:" << this;
  auto read_promise_moved = std::move(read_promise);
  if (error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "stream.async_read, this:" << this << ", error:" << error.message();
    reading = false;
    read_promise_moved->set_exception(boost::system::system_error(error));
    return;
  }
  std::string result = boost::beast::buffers_to_string(read_buffer->data());
  read_buffer->consume(read_buffer->size());
  read_promise_moved->set_value(result);
  reading = false;
}
