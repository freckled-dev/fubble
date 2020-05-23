#include "acceptor.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"

using namespace websocket;

acceptor::acceptor(boost::asio::io_context &context,
                   connection_creator &creator, const config &config_)
    : acceptor_(context), creator(creator) {
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),
                                          config_.port);
  acceptor_.open(endpoint.protocol());
  boost::asio::socket_base::reuse_address reuse_address(true);
  acceptor_.set_option(reuse_address);
  try {
    acceptor_.bind(endpoint);
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "failed to bind to the endpoint:" << endpoint;
    std::rethrow_exception(std::current_exception());
  }
  acceptor_.listen();

  run();
}

void acceptor::close() { acceptor_.close(); }

std::uint16_t acceptor::get_port() const {
  return acceptor_.local_endpoint().port();
}

void acceptor::run() {
  auto connection_ = creator.create(false);
  auto &native = connection_->get_native();
  boost::asio::ip::tcp::socket &tcp =
      std::get<connection::http_stream_type>(native).next_layer();
  acceptor_.async_accept(tcp, [this, connection_ = std::move(connection_)](
                                  const auto &error) mutable {
    if (error) {
      if (error == boost::asio::error::operation_aborted) {
        BOOST_LOG_SEV(this->logger, logging::severity::info)
            << "acceptor got closed";
        return;
      }
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "an error occured in tcp accept. what:" << error.message();
      return;
    }
    run();
    successful_tcp(std::move(connection_));
  });
}

void acceptor::successful_tcp(connection_ptr connection_parameter) {
  auto &native = std::get<connection::http_stream_type>(
      connection_parameter->get_native());
  native.async_accept([this, connection_ = std::move(connection_parameter)](
                          const auto &error) mutable {
    if (error) {
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "an error occured in websocket accept. what:" << error.message();
      return;
    }
    on_connection(connection_);
  });
}
