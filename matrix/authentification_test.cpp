#include "fixture.hpp"
#include "uuid.hpp"

using namespace matrix;

struct Authentifification : fixture {};

TEST_F(Authentifification, Instance) {}

TEST_F(Authentifification, RegisterAsGuest) {
  bool called{};
  auto result =
      authentification_.register_as_guest().then(executor, [&](auto result) {
        auto got_result = result.get();
        called = true;
        EXPECT_TRUE(got_result);
        context.stop();
      });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}

TEST_F(Authentifification, RegisterAsUser) {
  bool called{};
  auto username = uuid::generate();
  auto password = uuid::generate();
  auto result = authentification_.register_(username, password)
                    .then(executor, [&](auto result) {
                      auto got_result = result.get();
                      called = true;
                      EXPECT_TRUE(got_result);
                      context.stop();
                    });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}
