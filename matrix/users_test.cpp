#include "fixture.hpp"

struct Users : fixture {};

TEST_F(Users, Fun) {
  auto client_future = authentification_.register_anonymously();
  run_context();
  auto client_ = client_future.get();
  client_->sync();
  run_context();
  auto &users_ = client_->get_users();
}

