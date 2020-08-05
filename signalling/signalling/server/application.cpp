#include "application.hpp"
#include "signalling/device/creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/registration_handler.hpp"
#include "signalling/server/connection_creator.hpp"
#include "signalling/server/server.hpp"
#include "utils/executor_asio.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>

using namespace signalling ::server;

namespace {
class application_impl : public application {
public:
  application_impl(boost::asio::io_context &context, int port)
      : context(context), acceptor_config{static_cast<std::uint16_t>(port)} {}

  int get_port() const override { return server.get_port(); }

  void close() override { server.close(); }

protected:
  boost::asio::io_context &context;
  websocket::acceptor::config acceptor_config;
  using executor_type = boost::executor_adaptor<executor_asio>;
  executor_type asio_executor{context};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::acceptor websocket_acceptor{context, websocket_connection_creator,
                                         acceptor_config};
  signalling::json_message signalling_json;
  signalling::server::connection_creator server_connection_creator{
      asio_executor, signalling_json};
  signalling::device::creator device_creator_{asio_executor};
  signalling::registration_handler registration_handler{device_creator_};
  signalling::server::server server{asio_executor, websocket_acceptor,
                                    server_connection_creator,
                                    registration_handler};
};
} // namespace

std::unique_ptr<application>
application::create(boost::asio::io_context &context, int port) {
  return std::make_unique<application_impl>(context, port);
}
