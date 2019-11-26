#include "signalling/client/client.hpp"
#include "executor_asio.hpp"
#include "exit_signals.hpp"
#include "logging/initialser.hpp"
#include "options.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::info) << "starting";

  options options_parser;
  auto parsed_config = options_parser(argc, argv);
  if (!parsed_config)
    return 0;
  const auto config_ = parsed_config.value();

  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector websocket_connector{context, boost_executor,
                                           websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client signaling_client{
      boost_executor, websocket_connector, signalling_connection_creator};

  signaling_client(config_.signalling_.host, config_.signalling_.service,
                   config_.signalling_.id);

  exit_signals signals_{executor};
  signals_.async_wait([&](auto &error) {
    if (error)
      return;
    signaling_client.close();
  });

  context.run();
  return 0;
}
