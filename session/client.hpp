#ifndef UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E
#define UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E

#include "logging/logger.hpp"
#include <boost/asio/executor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
#include <nakama-cpp/Nakama.h>

namespace session {
class client : public Nakama::NRtClientListenerInterface {
public:
  client(boost::asio::executor &executor);

  void set_name(const std::string &name);
  boost::signals2::signal<void()> on_connected;
  boost::signals2::signal<void()> on_disconnected;

  Nakama::NRtClientPtr get_native_realtime_client() const;
  Nakama::NClientPtr get_native_client() const;
  Nakama::NSessionPtr get_native_session() const;
  boost::signals2::signal<void(const Nakama::NChannelMessage &)>
      on_channel_message;
  boost::signals2::signal<void(const Nakama::NChannelPresenceEvent &)>
      on_channel_presence;

protected:
  void post_tick();
  void on_tick(const boost::system::error_code &error);
  void tick();
  void on_logged_in(const Nakama::NSessionPtr &session_);
#if 0
  void on_login_failed(const Nakama::NError &error);
#endif
  void on_nakama_error(const Nakama::NError &error);
  void set_display_name();
  void onConnect() override;
  void onDisconnect(const Nakama::NRtClientDisconnectInfo &info) override;
  void onChannelMessage(const Nakama::NChannelMessage &message) override;
  void
  onChannelPresence(const Nakama::NChannelPresenceEvent &presence) override;

  logging::logger logger;
  boost::asio::executor &executor;
  boost::asio::steady_timer tick_timer{executor};
  std::string name;
  Nakama::NClientPtr client_;
  Nakama::NRtClientPtr realtime_client;
  Nakama::NSessionPtr session_;
};
} // namespace session

#endif
