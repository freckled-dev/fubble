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
  acceptor_.bind(endpoint);
  acceptor_.listen();
}

void acceptor::close() { acceptor_.close(); }

std::unique_ptr<connection> acceptor::
operator()(boost::asio::yield_context yield) {
  auto result = creator();
  auto &native = result->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  acceptor_.async_accept(tcp, yield);
  native.async_accept(yield);
  return result;
}

std::uint16_t acceptor::get_port() const {
  return acceptor_.local_endpoint().port();
}
