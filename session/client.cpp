#include "client.hpp"
#include "uuid.hpp"

using namespace session;

client::client(boost::asio::executor &executor)
    : executor(executor), tick_timer(executor) {
  natives_.client_ = Nakama::createDefaultClient(Nakama::NClientParameters{});
  natives_.client_->setErrorCallback(
      [this](const auto &error) { on_nakama_error(error); });
  natives_.realtime_client = natives_.client_->createRtClient();
  natives_.realtime_client->setListener(&natives_.realtime_signals);
  post_tick();
}

client::~client() = default;

void client::close() {
  tick_timer.cancel();
  if (natives_.realtime_client)
    natives_.realtime_client->disconnect();
  natives_.client_->disconnect();
  natives_.session_.reset();
}

void client::set_name(const std::string &name_) {
  if (name == name_)
    return;
  name = name_;
  set_display_name();
}

const client::natives &client::get_natives() const { return natives_; }
client::natives &client::get_natives() { return natives_; }

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
  natives_.client_->tick();
  if (natives_.realtime_client)
    natives_.realtime_client->tick();
  post_tick();
}

void client::on_nakama_error(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_login_failed, error:" << error.message;
}

void client::set_display_name() {
  if (!natives_.session_ || name.empty())
    return;
  auto username = Nakama::opt::nullopt;
  natives_.client_->updateAccount(natives_.session_, username, name);
}

