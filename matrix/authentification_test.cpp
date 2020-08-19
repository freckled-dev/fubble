#include "fixture.hpp"
#include "uuid.hpp"

using namespace matrix;

namespace {
struct Authentifification : fixture {
  authentification::client_ptr register_(std::string username,
                                         std::string password) {
    auto result_future = authentification_.register_(username, password);
    run_context();
    auto result = result_future.get();
    EXPECT_TRUE(result);
    return result;
  }
  authentification::client_ptr login(std::string username,
                                     std::string password) {
    authentification::user_login_information information;
    information.username = username;
    information.password = password;
    auto result_future = authentification_.login(information);
    run_context();
    auto result = result_future.get();
    EXPECT_TRUE(result);
    return result;
  }
};
} // namespace

TEST_F(Authentifification, Instance) {}

TEST_F(Authentifification, RegisterAsGuest) {
  bool called{};
  auto result =
      authentification_.register_as_guest().then(executor, [&](auto result) {
        auto got_result = result.get();
        called = true;
        EXPECT_TRUE(got_result);
      });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}

TEST_F(Authentifification, RegisterAnonymously) {
  bool called{};
  auto result =
      authentification_.register_anonymously().then(executor, [&](auto result) {
        auto got_result = result.get();
        called = true;
        EXPECT_TRUE(got_result);
      });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}

TEST_F(Authentifification, RegisterAsUser) {
  auto username = uuid::generate();
  auto password = uuid::generate();
  register_(username, password);
}

TEST_F(Authentifification, RegisterSameUser) {
  auto username = uuid::generate();
  auto password = uuid::generate();
  register_(username, password);
  EXPECT_THROW(register_(username, password), boost::exception);
}

TEST_F(Authentifification, LoginFail) {
  auto username = uuid::generate();
  auto password = uuid::generate();
  EXPECT_THROW(login(username, password), boost::exception);
}

TEST_F(Authentifification, Login) {
  auto username = uuid::generate();
  auto password = uuid::generate();
  register_(username, password);
  login(username, password);
}
