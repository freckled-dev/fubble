#include "fixture.hpp"

using namespace matrix;

struct Rooms : fixture {};

TEST_F(Rooms, CreateRoom) {
  auto test = authentification_.register_anonymously();
  bool called{};
  std::unique_ptr<client> client_;
  auto done = test.then(executor,
                        [&](auto client_future) {
                          client_ = std::move(client_future.get());
                          return client_.get()->get_rooms().create_room();
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

TEST_F(Rooms, InviteByUserId) {
  auto inviter_future = authentification_.register_anonymously();
  auto invitee_future = authentification_.register_as_guest();
  run_context();
  auto inviter = inviter_future.get();
  auto invitee = invitee_future.get();
  auto room_future = inviter->get_rooms().create_room();
  run_context();
  auto room = room_future.get();
  auto join_fail = invitee->get_rooms().join_room_by_id(room->get_id());
  run_context();
  EXPECT_THROW(join_fail.get(), error::response);
  auto invite_future = room->invite_by_user_id(invitee->get_user_id());
  run_context();
  invite_future.get();
  auto join_success = invitee->get_rooms().join_room_by_id(room->get_id());
  run_context();
  EXPECT_EQ(join_success.get()->get_id(), room->get_id());
}

TEST_F(Rooms, SyncRoomJoin) {
  auto inviter_future = authentification_.register_anonymously();
  auto invitee_future = authentification_.register_as_guest();
  run_context();
  auto inviter = inviter_future.get();
  auto invitee = invitee_future.get();
  inviter->sync();
  invitee->sync();
  auto room_future = inviter->get_rooms().create_room();
  run_context();
  auto room = room_future.get();
  room->invite_by_user_id(invitee->get_user_id());
  run_context();
  auto invitee_room_future =
      invitee->get_rooms().join_room_by_id(room->get_id());
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

TEST_F(Rooms, DisplayName) {
  auto client_ = create_registered_client();
  client_->get_rooms().create_room();
  run_context();
  sync_client(*client_);
  auto user_id = client_->get_user_id();
  const auto check_last = user_id.find(":");
  const std::string check = user_id.substr(1, check_last - 1);
  EXPECT_EQ(client_->get_users().get_all().front()->get_display_name(), check);
}

TEST_F(Rooms, Leave) {
  auto client_ = create_registered_client();
  auto room_future = client_->get_rooms().create_room();
  run_context();
  auto room_ = room_future.get();
  sync_client(*client_);
  auto leave_future = client_->get_rooms().leave_room(*room_);
  run_context();
  leave_future.get();
  sync_client(*client_);
  EXPECT_TRUE(client_->get_rooms().get_rooms().empty());
}

TEST_F(Rooms, Kick) {
  auto inviter = create_registered_client();
  auto invitee = create_guest_client();
  auto room_future = inviter->get_rooms().create_room();
  run_context();
  auto room_ = room_future.get();
  room_->invite_by_user_id(invitee->get_user_id());
  run_context();
  invitee->get_rooms().join_room_by_id(room_->get_id());
  sync_client(*invitee);
  bool called{};
  invitee->get_rooms().on_leave.connect([&](auto room_id) {
    EXPECT_EQ(room_id, room_->get_id());
    called = true;
  });
  EXPECT_EQ(invitee->get_rooms().get_rooms().size(), 1);
  auto kick_future = room_->kick(invitee->get_user_id());
  run_context();
  kick_future.get();
  sync_client(*invitee);
  EXPECT_TRUE(invitee->get_rooms().get_rooms().empty());
  EXPECT_TRUE(called);
}
