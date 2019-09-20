#include "connection.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

websocket::connection::connection(boost::asio::io_context& context)
    : socket{context} {}

websocket::connection::~connection() = default;

void websocket::connection::read_async(
    const connection::callback_message_type& callback_message_) {
  callback_message = callback_message_;
  boost::asio::spawn([this](boost::asio::yield_context yield) {
    try {
      read_async(yield);
    } catch (const boost::system::system_error& error) {
      std::cerr << "there was an error with read_async:" << error.what()
                << std::endl;
    }
  });
}

void websocket::connection::write(const connection::message_type& message) {
  const auto buffer = boost::asio::buffer(message);
  stream.write(buffer);
}

void websocket::connection::read_async(boost::asio::yield_context yield) {
  while (true) {
    boost::beast::multi_buffer buffer;
    stream.async_read(buffer, yield);
    std::string message_casted = boost::beast::buffers_to_string(buffer.data());
    std::cout << "connection::read_async, message_casted:" << message_casted
              << std::endl;
    callback_message(message_casted);
  }
}
