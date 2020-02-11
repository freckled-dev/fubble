#include "client.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

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

client::~client() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~client()";
}

void client::close() {
  tick_timer.cancel();
  if (natives_.realtime_client)
    natives_.realtime_client->disconnect();
  natives_.client_->disconnect();
  natives_.session_.reset();
}

boost::future<void> client::set_name(const std::string &name) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "setting name to:" << name;
  if (!natives_.session_ || name.empty()) {
    BOOST_ASSERT(false);
    return boost::make_exceptional_future<void>(
        std::runtime_error("could not set name, because we are not connected"));
  }
  auto username = Nakama::opt::nullopt;
  auto avatarUrl = Nakama::opt::nullopt;
  auto langTag = Nakama::opt::nullopt;
  auto location = Nakama::opt::nullopt;
  auto timezone = Nakama::opt::nullopt;
  auto promise = std::make_shared<boost::promise<void>>();
  auto on_success = [this, promise] {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "name got set";
    promise->set_value();
  };
  auto on_failure = [this, promise](auto error) {
    auto message =
        fmt::format("could not set name, message: {}", error.message);
    BOOST_LOG_SEV(logger, logging::severity::warning) << message;
    promise->set_exception(std::runtime_error(message));
  };
  natives_.client_->updateAccount(natives_.session_, username, name, avatarUrl,
                                  langTag, location, timezone,
                                  std::move(on_success), std::move(on_failure));
  return promise->get_future();
}

const client::natives &client::get_natives() const { return natives_; }
client::natives &client::get_natives() { return natives_; }

void client::post_tick() {
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "post_tick";
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
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "tick";

  natives_.client_->tick();
  if (natives_.realtime_client)
    natives_.realtime_client->tick();
  // BOOST_LOG_SEV(logger, logging::severity::trace) << "tick after native";
  post_tick();
}

void client::on_nakama_error(const Nakama::NError &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "on_login_failed, error:" << error.message;
}
