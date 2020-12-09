#include "application.hpp"
#include "signaling/device/creator.hpp"
#include "signaling/json_message.hpp"
#include "signaling/registration_handler.hpp"
#include "signaling/server/connection_creator.hpp"
#include "signaling/server/server.hpp"
#include "utils/executor_asio.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>

using namespace signaling::server;

namespace {
class server_impl : public application {
public:
  server_impl(boost::asio::io_context &context, int port)
      : context(context), acceptor_config{static_cast<std::uint16_t>(port)} {}

  int get_port() const override { return server_.get_port(); }

  void close() override { server_.close(); }

  signaling::server::server &get_server() override { return server_; }

  signaling::registration_handler &get_registrations() override {
    return registration_handler;
  }

protected:
  boost::asio::io_context &context;
  websocket::acceptor::config acceptor_config;
  using executor_type = boost::executor_adaptor<executor_asio>;
  executor_type asio_executor{context};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::acceptor websocket_acceptor{context, websocket_connection_creator,
                                         acceptor_config};
  signaling::json_message signaling_json;
  signaling::server::connection_creator server_connection_creator{
      asio_executor, signaling_json};
  signaling::device::creator device_creator_{asio_executor};
  signaling::registration_handler registration_handler{device_creator_};
  signaling::server::server server_{asio_executor, websocket_acceptor,
                                     server_connection_creator,
                                     registration_handler};
};
} // namespace

std::unique_ptr<application>
application::create(boost::asio::io_context &context, int port) {
  return std::make_unique<server_impl>(context, port);
}
