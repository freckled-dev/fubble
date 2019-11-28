#include "signalling/client/client.hpp"
#include "executor_asio.hpp"
#include "exit_signals.hpp"
#include "logging/initialser.hpp"
#include "options.hpp"
#include "rtc/connection.hpp"
#include "rtc/google/connection_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/asio/io_context.hpp>

namespace {
struct offer_answer_handler {
  boost::executor &executor;
  signalling::client::client &signalling_client;
  rtc::connection &rtc_connection;
  bool offering{true};

  offer_answer_handler(boost::executor &executor,
                       signalling::client::client &signalling_client,
                       rtc::connection &rtc_connection, bool offering)
      : executor(executor), signalling_client(signalling_client),
        rtc_connection(rtc_connection), offering(offering) {
    rtc_connection.on_negotiation_needed.connect([this] { renegotiate(); });
    signalling_client.on_offer.connect([&](auto sdp) { on_offer(sdp); });
    signalling_client.on_answer.connect([&](auto sdp) { on_answer(sdp); });
  }

  void renegotiate() {
    if (!offering)
      return;
    auto sdp = rtc_connection.create_offer();
    sdp.then(executor, [this](auto offer_future) {
      auto offer = offer_future.get();
      rtc_connection.set_local_description(offer);
      signalling::offer offer_casted{offer.sdp};
      signalling_client.send_offer(offer_casted);
    });
  }

  void on_answer(signalling::answer sdp) {
    rtc::session_description answer_casted{
        rtc::session_description::type::answer, sdp.sdp};
    rtc_connection.set_remote_description(answer_casted);
  }

  void on_offer(signalling::offer sdp) {
    rtc::session_description sdp_casted{rtc::session_description::type::offer,
                                        sdp.sdp};
    rtc_connection.set_remote_description(sdp_casted);
    rtc_connection.create_answer().then(executor, [this](auto answer_future) {
      auto answer = answer_future.get();
      rtc_connection.set_local_description(answer);
      signalling::answer answer_casted{answer.sdp};
      signalling_client.send_answer(answer_casted);
    });
  }
};
struct ice_candidate_handler {
  signalling::client::client &signalling_client;
  rtc::connection &rtc_connection;
  ice_candidate_handler(signalling::client::client &signalling_client,
                        rtc::connection &rtc_connection)
      : signalling_client(signalling_client), rtc_connection(rtc_connection) {
    rtc_connection.on_ice_candidate.connect(
        [this](const rtc::ice_candidate &candidate) { rtc(candidate); });
    signalling_client.on_ice_candidate.connect(
        [this](auto candidate) { signalling(candidate); });
  }
  void rtc(const rtc::ice_candidate &candidate) {
    signalling::ice_candidate candidate_casted{candidate.mlineindex,
                                               candidate.mid, candidate.sdp};
    signalling_client.send_ice_candidate(candidate_casted);
  }
  void signalling(const signalling::ice_candidate &candidate) {
    rtc::ice_candidate candidate_casted{candidate.mlineindex, candidate.mid,
                                        candidate.sdp};
    rtc_connection.add_ice_candidate(candidate_casted);
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
  signalling::client::client signaling_client{
      boost_executor, websocket_connector, signalling_connection_creator};
  signaling_client.on_error.connect([&](auto /*error*/) { signals_.close(); });

  rtc::google::connection_creator rtc_connection_creator;
  std::unique_ptr<rtc::connection> rtc_connection = rtc_connection_creator();

  signaling_client.on_create_offer.connect([&] {
    // TODO create rtc connection
  });
  signaling_client(config_.signalling_.host, config_.signalling_.service,
                   config_.signalling_.id);

  signals_.async_wait([&](auto &error) {
    if (error)
      return;
    signaling_client.close();
  });

  context.run();
  return 0;
}
