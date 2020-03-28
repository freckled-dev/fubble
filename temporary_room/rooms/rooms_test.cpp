#include "rooms.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace temporary_room::rooms;

class mock_room_factory : public room_factory {
public:
  MOCK_METHOD(boost::future<room_ptr>, create, (), (override));
};

class mock_room : public room {
public:
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
  MOCK_METHOD(room_id, get_room_id, (), (const override));
};

TEST(Rooms, Instance) {
  mock_room_factory room_factory_;
  rooms test{room_factory_};
}

TEST(Rooms, Add) {
  mock_room_factory room_factory_;
  const room_id room_id_{"fun_id"};
  auto room_ = std::make_unique<mock_room>();
  EXPECT_CALL(*room_, get_room_id).WillOnce(::testing::Return(room_id_));
  room_ptr room_casted = std::move(room_);
  EXPECT_CALL(room_factory_, create)
      .WillOnce(::testing::Return(
          ::testing::ByMove(boost::make_ready_future(std::move(room_casted)))));
  rooms test{room_factory_};
  auto add_future = test.get_or_create_room_id("name");
  EXPECT_EQ(add_future.get(), room_id_);
  EXPECT_EQ(test.get_room_count(), 1);
}

TEST(Rooms, AddTwoParticipants) {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  boost::promise<room_ptr> create_promise;
  EXPECT_CALL(room_factory_, create)
      .WillOnce(
          ::testing::Return(::testing::ByMove(create_promise.get_future())));
  rooms test{room_factory_};
  int called{};
  for (int counter{}; counter < 2; ++counter) {
    test.get_or_create_room_id("name").then([&](auto result) {
      result.get();
      ++called;
    });
  }
  EXPECT_EQ(test.get_room_count(), 1);
  auto room_ = std::make_unique<mock_room>();
  EXPECT_CALL(*room_, get_room_id).WillOnce(::testing::Return(room_id_));
  create_promise.set_value(std::move(room_));
  EXPECT_EQ(test.get_room_count(), 1);
}

MATCHER_P(HasCorrectError, message, "") { return arg.what() == message; }

TEST(Rooms, AddFail) {
  mock_room_factory room_factory_;
  std::string error_message = "failed";
  EXPECT_CALL(room_factory_, create)
      .WillOnce(::testing::Return(
          ::testing::ByMove(boost::make_exceptional_future<room_ptr>(
              std::runtime_error(error_message)))));
  rooms test{room_factory_};
  auto result = test.get_or_create_room_id("name");
  EXPECT_THROW(result.get(), std::runtime_error);
}

struct RoomWithParticipants : testing::Test {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  room_name room_name_{"room_name"};
  rooms test{room_factory_};
  std::vector<boost::future<room_id>> participants;
  room *room_;
  std::shared_ptr<int> room_alive_check;

  RoomWithParticipants() {
    auto room_ = std::make_unique<mock_room>();
    this->room_ = room_.get();
    room_alive_check = room_->alive_check;
    EXPECT_CALL(*room_, get_room_id)
        .WillRepeatedly(::testing::Return(room_id_));
    room_ptr room_casted = std::move(room_);
    EXPECT_CALL(room_factory_, create)
        .WillOnce(::testing::Return(::testing::ByMove(
            boost::make_ready_future(std::move(room_casted)))));
  }
  void add_participants(int count) {
    for (int index = 0; index < count; ++index)
      participants.emplace_back(test.get_or_create_room_id(room_name_));
  }

  void remove_all_participants() { room_->on_participant_count_changed(1); }

  void ensure_room_id_got_set() {
    for (auto &participant_ : participants)
      EXPECT_EQ(participant_.get(), room_id_);
  }
};

TEST_F(RoomWithParticipants, Remove) {
  add_participants(1);
  ensure_room_id_got_set();
  remove_all_participants();
  EXPECT_TRUE(room_alive_check.unique());
  EXPECT_EQ(test.get_room_count(), 0);
}

TEST_F(RoomWithParticipants, RemoveTwo) {
  add_participants(2);
  ensure_room_id_got_set();
  remove_all_participants();
  EXPECT_TRUE(room_alive_check.unique());
  EXPECT_EQ(test.get_room_count(), 0);
}
