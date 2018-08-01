#include "server.hpp"
#include "connection.hpp"

websocket::server::server(boost::asio::io_context &context, const port port_,
                          const server::async_accept_callback &callback)
    : context(context),
      endpoint(boost::asio::ip::tcp::v4(), port_.value),
      acceptor(context, endpoint),
      callback{callback} {}

void websocket::server::start() {
  boost::asio::spawn(
      [this](boost::asio::yield_context yield) { start(yield); });
}

void websocket::server::stop() {}

void websocket::server::async_accept(boost::asio::yield_context yield) {
  auto connection_ = std::make_unique<websocket::connection>(context);
  acceptor.async_accept(connection_->socket, yield);
  connection_->stream.async_accept(yield);
  std::unique_ptr<::connection> connection_casted = std::move(connection_);
  callback(std::move(connection_casted));
}

void websocket::server::start(boost::asio::yield_context yield) {
  while (true) {
    // try {
    async_accept(yield);
    // } catch (const std::exception &error) {
    //   std::cout << "there was an error on start, what:" << error.what()
    //             << std::endl;
    // }
  }
}
