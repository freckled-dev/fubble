#include "fixture.hpp"

struct Users : fixture {};

TEST_F(Users, Sync) {
  auto client_future = authentification_.register_anonymously();
  run_context();
  auto client_ = client_future.get();
  client_->sync();
  run_context();
  auto &users_ = client_->get_users();
  const auto &all = users_.get_all();
  EXPECT_EQ(all.size(), 1);
  const auto &first = all.front();
  EXPECT_EQ(first->get_id(), client_->get_user_id());
  EXPECT_EQ(first->get_presence(), "online");
}

