#include "logging/logger.hpp"
#include "uuid.hpp"
#include <boost/signals2/signal.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <nakama-cpp/Nakama.h>
#include <thread>

TEST(Nakama, CreateDefaultClient) {
  using namespace NAKAMA_NAMESPACE;
  NClientParameters parameters;
#if 0
  parameters.serverKey = "defaultkey";
  parameters.host = "127.0.0.1";
  parameters.port = DEFAULT_PORT;
#endif
  NClientPtr client = createDefaultClient(parameters);
  EXPECT_TRUE(client);
}

namespace {
logging::logger logger{"nakama_test"};
std::string generate_device_id() { return uuid::generate(); }
struct session {
  Nakama::NClientPtr client =
      Nakama::createDefaultClient(Nakama::NClientParameters{});
  Nakama::NSessionPtr session_;
  session(const std::string name) {
    using namespace NAKAMA_NAMESPACE;
    auto loginFailedCallback = [&](const NError &) { EXPECT_TRUE(false); };
    bool called{};
    auto loginSucceededCallback = [&](NSessionPtr session_) {
      EXPECT_TRUE(session_);
      this->session_ = session_;
      called = true;
    };
    const std::string device_id = generate_device_id();
    client->authenticateDevice(device_id, opt::nullopt, opt::nullopt, {},
                               loginSucceededCallback, loginFailedCallback);
    while (!called)
      tick_50ms();
    called = false;
    client->updateAccount(session_, Nakama::opt::nullopt, name,
                          Nakama::opt::nullopt, Nakama::opt::nullopt,
                          Nakama::opt::nullopt, Nakama::opt::nullopt,
                          [&] { called = true; });
    while (!called)
      tick_50ms();
    EXPECT_TRUE(called);
  }
  void tick_50ms() {
    BOOST_LOG_SEV(logger, logging::severity::info) << "tick_50ms";
    client->tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
};
} // namespace

TEST(Nakama, AuthenticateDevice) { session test{"name"}; }

namespace {
struct realtime : Nakama::NRtClientListenerInterface {
  session &session_;
  // WATCHOUT this structure will NOT be updated, if another person joins the
  // room
  Nakama::NRtClientPtr realtime_client;
  boost::signals2::signal<void()> on_connected;
  boost::signals2::signal<void()> on_joins;
  boost::signals2::signal<void(std::string from, std::string content)>
      on_message;
  realtime(session &session_) : session_(session_) {
    realtime_client = session_.client->createRtClient();
    realtime_client->setListener(this);
    bool show_as_online = true;
    realtime_client->connect(session_.session_, show_as_online);
  }
  void onConnect() override {
    // BOOST_LOG_SEV(logger, logging::severity::info) << "onConnect";
    on_connected();
  }
  void onDisconnect(const Nakama::NRtClientDisconnectInfo &) override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "onDisconnect";
  }
  void onChannelMessage(const Nakama::NChannelMessage &message) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "got a message, content:" << message.content;
    on_message(message.senderId, message.content);
  }
  virtual void
  onChannelPresence(const Nakama::NChannelPresenceEvent &presence) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("presences got updated, joins:{}, leaves:{}",
                       presence.joins.size(), presence.leaves.size());
    for (auto &join : presence.joins) {
      BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
          "join.userName:{}, join.userId:{}", join.username, join.userId);
    }
    if (!presence.joins.empty())
      on_joins();
  }
  void onError(const Nakama::NRtError &) override { EXPECT_TRUE(false); }
  void tick_50ms() {
    // BOOST_LOG_SEV(logger, logging::severity::info) << "tick_50ms realtime";
    realtime_client->tick();
    session_.tick_50ms();
  }
  void wait_for_connected() {
    bool wait{};
    boost::signals2::scoped_connection connection =
        on_connected.connect([&] { wait = true; });
    while (!wait)
      tick_50ms();
  }
};
} // namespace

TEST(Nakama, Realtime) {
  session session_{"name"};
  realtime realtime_{session_};
  realtime_.wait_for_connected();
}

namespace {
struct join_chat {
  realtime &realtime_;
  Nakama::NChannelPtr channel;
  join_chat(realtime &realtime_, std::string id) : realtime_(realtime_) {
    realtime_.realtime_client->joinChat(
        id, Nakama::NChannelType::ROOM, Nakama::opt::nullopt,
        Nakama::opt::nullopt, [this](auto session) { on_success(session); });
  }
  void on_success(Nakama::NChannelPtr channel) { this->channel = channel; }
  void wait_for_joined() {
    while (!channel)
      realtime_.tick_50ms();
  }
};
} // namespace

TEST(Nakama, JoinChat) {
  session session_{"name"};
  realtime realtime_{session_};
  realtime_.wait_for_connected();
  join_chat joiner{realtime_, "fun"};
  joiner.wait_for_joined();
  EXPECT_TRUE(joiner.channel);
  EXPECT_TRUE(joiner.channel->presences.empty());
  EXPECT_EQ(joiner.channel->roomName, "fun");
  EXPECT_FALSE(joiner.channel->id.empty());
}

TEST(Nakama, JoinChatSpecialId) {
  session session_{"name"};
  realtime realtime_{session_};
  realtime_.wait_for_connected();
  const std::string name = u8"😁";
  join_chat joiner{realtime_, name};
  joiner.wait_for_joined();
  EXPECT_TRUE(joiner.channel);
  EXPECT_EQ(joiner.channel->roomName, name);
}

TEST(Nakama, ChatTwoParticipants) {
  session first_session{"first"};
  realtime first_realtime{first_session};
  first_realtime.wait_for_connected();
  join_chat first_joiner{first_realtime, "fun"};
  first_joiner.wait_for_joined();
  session second_session{"second"};
  realtime second_realtime{second_session};
  second_realtime.wait_for_connected();
  join_chat second_joiner{second_realtime, "fun"};
  second_joiner.wait_for_joined();
  EXPECT_EQ(second_joiner.channel->presences.size(), std::size_t{1});
}

TEST(Nakama, ChatJoinEvent) {
  session first_session{"first"};
  realtime first_realtime{first_session};
  bool joins_called{};
  first_realtime.on_joins.connect([&] { joins_called = true; });
  first_realtime.wait_for_connected();
  join_chat first_joiner{first_realtime, "fun"};
  first_joiner.wait_for_joined();
  session second_session{"second"};
  realtime second_realtime{second_session};
  second_realtime.wait_for_connected();
  join_chat second_joiner{second_realtime, "fun"};
  second_joiner.wait_for_joined();
  while (!joins_called) {
    first_realtime.tick_50ms();
    second_realtime.tick_50ms();
  }
}

namespace {
struct NakamaTwoSessions : testing::Test {
  std::string roomName = "fun";
  session first_session{"first"};
  realtime first_realtime{first_session};
  session second_session{"second"};
  realtime second_realtime{second_session};
  std::string channel_id;
  NakamaTwoSessions() {
    first_realtime.wait_for_connected();
    second_realtime.wait_for_connected();
    join_chat first_joiner{first_realtime, roomName};
    join_chat second_joiner{second_realtime, roomName};
    first_joiner.wait_for_joined();
    second_joiner.wait_for_joined();
    EXPECT_EQ(first_joiner.channel->id, second_joiner.channel->id);
    channel_id = first_joiner.channel->id;
  }
  void tick_50ms() {
    first_realtime.tick_50ms();
    second_realtime.tick_50ms();
  }
};
} // namespace

TEST_F(NakamaTwoSessions, ChatMessage) {
  std::string content = "{ \"some\": \"data\" }";
  first_realtime.realtime_client->writeChatMessage(channel_id, content);
  bool called{};
  second_realtime.on_message.connect([&](auto, auto receivedContent) {
    EXPECT_EQ(receivedContent, content);
    called = true;
  });
  while (!called)
    tick_50ms();
}

