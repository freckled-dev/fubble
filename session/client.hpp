#ifndef UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E
#define UUID_D10FE831_B416_4A53_9EBF_0BA3F656F47E

#include "logging/logger.hpp"
#include "nakama_realtime_client_signals.hpp"
#include <boost/asio/executor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
#include <nakama-cpp/Nakama.h>

namespace session {
class client {
public:
  client(boost::asio::executor &executor);
  ~client();

  void close();
  void set_name(const std::string &name);
  boost::signals2::signal<void()> on_connected;
  boost::signals2::signal<void()> on_disconnected;

  struct natives {
    Nakama::NClientPtr client_;
    Nakama::NRtClientPtr realtime_client;
    Nakama::NSessionPtr session_;
    nakama_realtime_client_signals realtime_signals;
  };
  const natives &get_natives() const;
  natives &get_natives();

protected:
  void post_tick();
  void on_tick(const boost::system::error_code &error);
  void tick();
  using promise_ptr = std::shared_ptr<boost::promise<void>>;
  void on_logged_in(promise_ptr promise, const Nakama::NSessionPtr &session_);
  void on_login_failed(promise_ptr promise, const Nakama::NError &error);
  void on_nakama_error(const Nakama::NError &error);
  void set_display_name();

  logging::logger logger;
  boost::asio::executor &executor;
  boost::asio::steady_timer tick_timer{executor};
  std::string name;
  natives natives_;
};
} // namespace session

#endif
