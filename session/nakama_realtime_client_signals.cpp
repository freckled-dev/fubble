#include "nakama_realtime_client_signals.hpp"

using namespace session;

void nakama_realtime_client_signals::onError(const Nakama::NRtError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "onError";
  on_error(error);
}

void nakama_realtime_client_signals::onConnect() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "onConnect";
  on_connect();
}

void nakama_realtime_client_signals::onDisconnect(
    const Nakama::NRtClientDisconnectInfo &info) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "onDisconnect, code:" << info.code << ", message:" << info.reason;
  on_disconnect();
}

void nakama_realtime_client_signals::onChannelMessage(
    const Nakama::NChannelMessage &message) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "onChannelMessage";
  on_channel_message(message);
}

void nakama_realtime_client_signals::onChannelPresence(
    const Nakama::NChannelPresenceEvent &presence) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "onChannelPresence";
  on_channel_presence(presence);
}
