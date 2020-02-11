#include "client_connector.hpp"
#include "client.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

using namespace session;
using namespace session;

client_connector::client_connector(client &client_) : client_(client_) {}

boost::future<void> client_connector::connect() {
  const std::string device_id = uuid::generate();

  auto &natives = client_.get_natives();
  BOOST_ASSERT(natives.client_);
  natives.client_->authenticateDevice(
      device_id, Nakama::opt::nullopt, Nakama::opt::nullopt, {},
      [this](const auto &session_) { on_logged_in(session_); },
      [this](const auto &error) { on_login_failed(error); });
  return promise.get_future();
}

void client_connector::on_logged_in(const Nakama::NSessionPtr &session_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_logged_in";
  auto &natives = client_.get_natives();
  natives.session_ = session_;
  bool show_as_online = true;
  signal_connections.push_back(natives.realtime_signals.on_connect.connect(
      [this] { on_realtime_connected(); }));
  signal_connections.push_back(natives.realtime_signals.on_error.connect(
      [this](const auto &error) { on_realtime_error(error); }));
  natives.realtime_client->connect(session_, show_as_online);
}

void client_connector::on_login_failed(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_login_failed, error:" << error.message;
  promise.set_exception(std::runtime_error(error.message));
}

void client_connector::on_realtime_connected() {
  signal_connections.clear();
  promise.set_value();
}

void client_connector::on_realtime_error(const Nakama::NRtError &error) {
  signal_connections.clear();
  auto message = fmt::format(
      "realtime client could not connect, error.message:{}", error.message);
  BOOST_LOG_SEV(logger, logging::severity::warning) << message;
  promise.set_exception(std::runtime_error(message));
}
