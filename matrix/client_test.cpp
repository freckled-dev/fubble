#include "fixture.hpp"

using namespace matrix;

struct Client : fixture {};

TEST_F(Client, Instance) {}

TEST_F(Client, CreateRoom) {
  auto test = authentification_.register_anonymously();
  bool called{};
  std::unique_ptr<client> client_;
  auto done = test.then(executor,
                        [&](auto client_future) {
                          client_ = std::move(client_future.get());
                          return client_.get()->create_room();
                        })
                  .unwrap()
                  .then(executor, [&](auto room) {
                    auto got_room = room.get();
                    EXPECT_TRUE(got_room);
                    EXPECT_FALSE(got_room->get_id().empty());
                    called = true;
                    context.stop();
                  });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_FALSE(client_->get_user_id().empty());
  done.get();
}

TEST_F(Client, InviteByUserId) {
  auto inviter_future = authentification_.register_anonymously();
  auto invitee_future = authentification_.register_as_guest();
  run_context();
  auto inviter = inviter_future.get();
  auto invitee = invitee_future.get();
  auto room_future = inviter->create_room();
  run_context();
  auto room = room_future.get();
  auto join_fail = invitee->join_room_by_id(room->get_id());
  run_context();
  EXPECT_THROW(join_fail.get(), error::response);
  auto invite_future = room->invite_by_user_id(invitee->get_user_id());
  run_context();
  invite_future.get();
  auto join_success = invitee->join_room_by_id(room->get_id());
  run_context();
  EXPECT_EQ(join_success.get()->get_id(), room->get_id());
}

TEST_F(Client, Sync) {
  auto client_future = authentification_.register_anonymously();
  run_context();
  auto client_ = client_future.get();
  auto sync_future = client_->sync();
  run_context();
  sync_future.get();
}

TEST_F(Client, SyncRoomJoin) {
  auto inviter_future = authentification_.register_anonymously();
  auto invitee_future = authentification_.register_as_guest();
  run_context();
  auto inviter = inviter_future.get();
  auto invitee = invitee_future.get();
  inviter->sync();
  invitee->sync();
  auto room_future = inviter->create_room();
  run_context();
  auto room = room_future.get();
  room->invite_by_user_id(invitee->get_user_id());
  run_context();
  auto invitee_room_future = invitee->join_room_by_id(room->get_id());
  run_context();
  auto invitee_room = invitee_room_future.get();
  auto inviter_sync = inviter->sync(std::chrono::seconds(2));
  auto invitee_sync = invitee->sync(std::chrono::seconds(2));
  run_context();
  inviter_sync.get();
  invitee_sync.get();
  EXPECT_EQ(room->get_members().size(), 2);
  EXPECT_EQ(invitee_room->get_members().size(), 2);
}
