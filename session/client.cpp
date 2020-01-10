#include "client.hpp"
#include "uuid.hpp"

using namespace session;

client::client(boost::asio::executor &executor)
    : executor(executor), tick_timer(executor) {
  client_ = Nakama::createDefaultClient(Nakama::NClientParameters{});
  client_->setErrorCallback(
      [this](const auto &error) { on_nakama_error(error); });
  const std::string device_id = uuid::generate();
  client_->authenticateDevice(
      device_id, Nakama::opt::nullopt, Nakama::opt::nullopt, {},
      [this](const auto &session_) { on_logged_in(session_); }
#if 0
      , [this](const auto &error) { on_login_failed(error); }
#endif
  );
}

void client::set_name(const std::string &name_) {
  if (name == name_)
    return;
  name = name_;
  set_display_name();
}

Nakama::NRtClientPtr client::get_native_realtime_client() const {
  return realtime_client;
}
Nakama::NClientPtr client::get_native_client() const { return client_; }

void client::post_tick() {
  tick_timer.expires_from_now(std::chrono::milliseconds(50));
  tick_timer.async_wait([this](const auto &error) { on_tick(error); });
}

void client::on_tick(const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted)
    return;
  if (error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "on_tick error:" << error.message();
    return;
  }
  tick();
}
void client::tick() {
  client_->tick();
  if (realtime_client)
    realtime_client->tick();
  post_tick();
}
void client::on_logged_in(const Nakama::NSessionPtr &session_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_logged_in";
  this->session_ = session_;
  set_display_name();
  realtime_client = client_->createRtClient();
  realtime_client->setListener(this);
  bool show_as_online = true;
  realtime_client->connect(session_, show_as_online);
}

#if 0
void client::on_login_failed(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_login_failed, error:" << error.message;
}
#endif

void client::on_nakama_error(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_login_failed, error:" << error.message;
}

void client::set_display_name() {
  if (!session_ || name.empty())
    return;
  auto username = Nakama::opt::nullopt;
  client_->updateAccount(session_, username, name);
}

void client::onConnect() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "onConnect";
  on_connected();
}

void client::onDisconnect(const Nakama::NRtClientDisconnectInfo &info) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "onDisconnect, code:" << info.code << ", message:" << info.reason;
  on_disconnected();
}
