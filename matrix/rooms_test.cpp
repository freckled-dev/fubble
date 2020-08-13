#include "chat.hpp"
#include "fixture.hpp"
#include "room_participant.hpp"
#include "room_states.hpp"

using namespace matrix;

struct Rooms : fixture {
  rooms::create_room_fields create_room_fields;
  Rooms() { create_room_fields.name = "room name"; }

  std::pair<std::unique_ptr<client>, room *> register_and_create_room() {
    auto client_ = create_registered_client();
    auto room_future = client_->get_rooms().create_room(create_room_fields);
    run_context();
    auto room_ = room_future.get();
    sync_client(*client_);
    return std::make_pair(std::move(client_), room_);
  }

  std::pair<std::unique_ptr<client>, room *>
  create_and_invite_guest(room &room_) {
    auto invitee = create_guest_client();
    room_.invite_by_user_id(invitee->get_user_id());
    run_context();
    auto joined = invitee->get_rooms().join_room_by_id(room_.get_id());
    sync_client(*invitee);
    auto result_room = joined.get();
    return std::make_pair(std::move(invitee), result_room);
  }
};

TEST_F(Rooms, CreateRoom) {
  auto test = authentification_.register_anonymously();
  bool called{};
  std::unique_ptr<client> client_;
  auto done = test.then(executor,
                        [&](auto client_future) {
                          client_ = std::move(client_future.get());
                          return client_.get()->get_rooms().create_room(
                              create_room_fields);
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
  auto room_future = inviter->get_rooms().create_room(create_room_fields);
  run_context();
  auto room = room_future.get();
  auto join_fail = invitee->get_rooms().join_room_by_id(room->get_id());
  run_context();
  // TODO refactor http client error handling!!!
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
  auto room_future = inviter->get_rooms().create_room(create_room_fields);
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
  auto [client_, room_] = register_and_create_room();
  auto user_id = client_->get_user_id();
  const auto check_last = user_id.find(":");
  const std::string check = user_id.substr(1, check_last - 1);
  EXPECT_EQ(client_->get_users().get_all().front()->get_display_name(), check);
}

TEST_F(Rooms, RoomName) {
  auto [client_, room_] = register_and_create_room();
  EXPECT_EQ(room_->get_name(), create_room_fields.name.value());
}

TEST_F(Rooms, Leave) {
  auto [client_, room_] = register_and_create_room();
  auto leave_future = client_->get_rooms().leave_room(*room_);
  run_context();
  leave_future.get();
  sync_client(*client_);
  EXPECT_TRUE(client_->get_rooms().get_rooms().empty());
}

TEST_F(Rooms, Online) {
  auto [client_, room_] = register_and_create_room();
  auto members = room_->get_members();
  ASSERT_FALSE(members.empty());
  EXPECT_EQ(members.front()->get_user().get_presence(),
            matrix::presence::online);
}

TEST_F(Rooms,
       DISABLED_GoOffline) { // this test waits for a timeout and lasts longer
                             // than 30 seconds!
  auto [client_, room_] = register_and_create_room();

  auto invitee = create_guest_client();
  sync_client(*invitee); // ensure invitee gets online
  room_->invite_by_user_id(invitee->get_user_id());
  run_context();
  invitee->get_rooms().join_room_by_id(room_->get_id());
  sync_client(*invitee);
  sync_client(*client_);

  bool called{};
  auto &other_user = client_->get_users().get_by_id(invitee->get_user_id());
  EXPECT_EQ(other_user.get_presence(), presence::online);
  other_user.on_update.connect([&] {
    called = true;
    EXPECT_EQ(other_user.get_presence(), presence::offline);
    client_->stop_sync();
  });
#if 0 // guests are not allowed to set themselfs offline
  auto did_presence = invitee->set_presence(presence::offline);
  run_context();
  did_presence.get();
#endif
  auto done = client_->sync_till_stop();
  context.run();
  EXPECT_TRUE(called);
  done.get();
}

TEST_F(Rooms, Kick) {
  auto [inviter, room_] = register_and_create_room();
  auto [invitee, invitee_room] = create_and_invite_guest(*room_);
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

TEST_F(Rooms, JoinJoinLeaveJoin) {
  // join, leave, join leads to crash due to matrix leaves not being empty
  // issue 53 https://gitlab.com/acof/fubble/-/issues/53
  auto [first, first_room] = register_and_create_room();
  auto room_id = first_room->get_id();
  auto [second, second_room] = create_and_invite_guest(*first_room);
  auto leave_future = second->get_rooms().leave_room_by_id(room_id);
  run_context();
  leave_future.get();
  auto [third, third_room] = create_and_invite_guest(*first_room);
  sync_client(*third);
}

TEST_F(Rooms, ChatSend) {
  auto [first, first_room] = register_and_create_room();
  auto &chat_ = first_room->get_chat();
  auto result = chat_.send("fun");
  run_context();
  result.get();
  EXPECT_TRUE(chat_.get_messages().empty());
}

TEST_F(Rooms, ChatReceive) {
  auto [first, first_room] = register_and_create_room();
  auto &chat_ = first_room->get_chat();
  const std::string message = "fun";
  auto sent = chat_.send(message);
  run_context();
  sent.get();
  sync_client(*first);
  ASSERT_EQ(chat_.get_messages().size(), 1);
  auto chat_message = chat_.get_messages().front();
  EXPECT_EQ(chat_message.body, message);
  EXPECT_EQ(first->get_user_id(), chat_message.user_->get_id());
  EXPECT_LE(chat_message.timestamp, std::chrono::system_clock::now());
}

TEST_F(Rooms, CustomState) {
  auto [first, first_room] = register_and_create_room();
  auto &states = first_room->get_states();
  matrix::room_states::custom set;
  set.key = "key";
  set.type = "io.fubble.test";
  auto result = states.set_custom(set);
  run_context();
  result.get();
  EXPECT_FALSE(states.get_all_custom().empty());
}
