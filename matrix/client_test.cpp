#include "fixture.hpp"

using namespace matrix;

struct Client : fixture {};

TEST_F(Client, Instance) {}

TEST_F(Client, Sync) {
  auto client_future = authentification_.register_anonymously();
  run_context();
  auto client_ = client_future.get();
  auto sync_future = client_->sync();
  run_context();
  sync_future.get();
}

