#ifndef UUID_3097ADFC_B1A4_4CA7_80E2_DEE577390CEA
#define UUID_3097ADFC_B1A4_4CA7_80E2_DEE577390CEA

#include "logging/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <nakama-cpp/Nakama.h>

namespace session {
class nakama_realtime_client_signals
    : public Nakama::NRtClientListenerInterface {
public:
  boost::signals2::signal<void(Nakama::NRtError)> on_error;
  boost::signals2::signal<void()> on_connect;
  boost::signals2::signal<void()> on_disconnect;
  boost::signals2::signal<void(const Nakama::NChannelMessage &)>
      on_channel_message;
  boost::signals2::signal<void(const Nakama::NChannelPresenceEvent &)>
      on_channel_presence;

protected:
  void onError(const Nakama::NRtError &error) override;
  void onConnect() override;
  void onDisconnect(const Nakama::NRtClientDisconnectInfo &info) override;
  void onChannelMessage(const Nakama::NChannelMessage &message) override;
  void
  onChannelPresence(const Nakama::NChannelPresenceEvent &presence) override;

  logging::logger logger;
};
} // namespace session

#endif
