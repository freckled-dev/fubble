#include "signalling/client/client.hpp"
#include "executor_asio.hpp"
#include "exit_signals.hpp"
#include "logging/initialser.hpp"
#include "options.hpp"
#include "p2p/negotiation/ice_candidates.hpp"
#include "p2p/negotiation/offer_answer.hpp"
#include "rtc/connection.hpp"
#include "rtc/data_channel.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/connection.hpp"
#include "rtc/google/factory.hpp"
#include "rtc/track_ptr.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <iostream>

namespace {
#if 0
struct video_track_handler {
  rtc::google::connection &rtc_connection;
  rtc::google::video_track_ptr video_track;

  video_track_handler(rtc::connection &rtc_connection)
      : rtc_connection(rtc_connection) {
    rtc_connection.on_video_track.connect([this](auto track) {
      BOOST_ASSERT(!video_track);
      video_track = track;
      connect_signals();
    });
  }

  void add() {
    BOOST_ASSERT(!video_track);
    // data_channel = rtc_connection.create_data_channel();
    connect_signals();
  }
  void connect_signals() {
    // video_track->
  }
};
#endif
struct data_channel_handler {
  rtc::connection &rtc_connection;
  rtc::data_channel_ptr data_channel;
  boost::signals2::signal<void()> on_opened;
  boost::signals2::signal<void(rtc::message)> on_message;

  data_channel_handler(rtc::connection &rtc_connection)
      : rtc_connection(rtc_connection) {
    rtc_connection.on_data_channel.connect([this](auto channel) {
      BOOST_ASSERT(!data_channel);
      data_channel = channel;
      connect_signals();
    });
  }

  void add() {
    BOOST_ASSERT(!data_channel);
    data_channel = rtc_connection.create_data_channel();
    connect_signals();
  }

  void connect_signals() {
    BOOST_ASSERT(data_channel);
    data_channel->on_opened.connect([this] { on_opened(); });
    data_channel->on_message.connect(
        [this](rtc::message message) { on_message(message); });
  }
};
struct message_writer {
  boost::asio::posix::stream_descriptor input;
  data_channel_handler &data_channel;
  std::vector<std::byte> line;

  message_writer(boost::asio::executor &executor,
                 data_channel_handler &data_channel)
      : input{executor, ::dup(STDIN_FILENO)}, data_channel(data_channel) {
    data_channel.on_opened.connect([&] {
      start_reading();
      data_channel.data_channel->on_message.connect(
          [this](auto message_) { message(message_); });
    });
  }

  void message(const rtc::message &message) {
    std::cout << message.to_string() << std::flush;
  }

  void start_reading() {
    boost::asio::async_read_until(
        input, boost::asio::dynamic_buffer(line), '\n',
        [this](auto error, auto transferred) { read(error, transferred); });
  }

  void close() {
    boost::system::error_code ignore_error;
    input.close(ignore_error);
  }

  void read(boost::system::error_code error, std::size_t transferred) {
    if (error)
      return;
    rtc::message message{line, false};
    data_channel.data_channel->send(message);
    line.erase(line.cbegin(), line.cbegin() + transferred);
    start_reading();
  }
};
} // namespace

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

  exit_signals signals_{executor};

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector websocket_connector{context, boost_executor,
                                           websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client signalling_client{
      boost_executor, websocket_connector, signalling_connection_creator};
  signalling_client.on_error.connect([&](auto /*error*/) { signals_.close(); });

  rtc::google::factory rtc_connection_creator;
  std::unique_ptr<rtc::connection> rtc_connection =
      rtc_connection_creator.create_connection();

  client::p2p::negotiation::ice_candidates ice_candidate_handler_{
      signalling_client, *rtc_connection};
  client::p2p::negotiation::offer_answer offer_answer_handler_{
      boost_executor, signalling_client, *rtc_connection};
  data_channel_handler data_channel_handler_{*rtc_connection};
  message_writer message_writer_{executor, data_channel_handler_};

  if (config_.video_.send) {
    rtc::google::capture::video::enumerator enumerator;
  }

  signalling_client.on_create_offer.connect(
      [&] { data_channel_handler_.add(); });
  signalling_client.on_closed.connect([&] {
    message_writer_.close();
    rtc_connection->close();
  });
  signalling_client(config_.signalling_.host, config_.signalling_.service,
                    config_.signalling_.id);

  signals_.async_wait([&](auto &error) {
    if (error)
      return;
    message_writer_.close();
    rtc_connection->close();
    signalling_client.close();
  });

  context.run();
  BOOST_LOG_SEV(logger, logging::severity::trace) << "context.run() ended";
  return 0;
}
