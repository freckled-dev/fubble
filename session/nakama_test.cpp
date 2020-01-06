#include <gtest/gtest.h>
#include <nakama-cpp/Nakama.h>
#include <thread>
#if 0
#include <nakama-cpp-c-wrapper/NakamaWrapper.h>

#include "nakama-cpp-c-wrapper/NakamaWrapperImpl.h"
#endif

TEST(Nakama, CreateDefaultClient) {
  // using namespace Nakama;
  using namespace NAKAMA_NAMESPACE;
  NLogger::initWithConsoleSink(NLogLevel::Debug);
  NClientParameters parameters;
#if 0
  parameters.serverKey = "defaultkey";
  parameters.host = "127.0.0.1";
  parameters.port = DEFAULT_PORT;
#endif
  NClientPtr client = createDefaultClient(parameters);
  EXPECT_TRUE(client);
}

struct NakamaClient : testing::Test {
  Nakama::NClientPtr client =
      Nakama::createDefaultClient(Nakama::NClientParameters{});
};

TEST_F(NakamaClient, AuthenticateDevice) {
  // using namespace Nakama;
  using namespace NAKAMA_NAMESPACE;
#if 1
  auto loginFailedCallback = [&](const NError &) { EXPECT_TRUE(false); };
  bool called{};
  auto loginSucceededCallback = [&](NSessionPtr session) {
    EXPECT_TRUE(session);
    called = true;
  };
#endif
  std::string device_id = "unique_device_id";
#if 1
  client->authenticateDevice(device_id, Nakama::opt::nullopt,
                             Nakama::opt::nullopt, {}, loginSucceededCallback,
                             loginFailedCallback);
#endif
  while (!called) {
    client->tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
#if 1
  EXPECT_TRUE(called);
#endif
}
