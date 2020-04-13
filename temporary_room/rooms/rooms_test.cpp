#include "rooms.hpp"
#include "uuid.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace temporary_room::rooms;

class mock_room_factory : public room_factory {
public:
  MOCK_METHOD(boost::future<room_ptr>, create, (const std::string &),
              (override));
};

class mock_room : public room {
public:
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
  MOCK_METHOD(room_id, get_room_id, (), (const override));
  MOCK_METHOD(boost::future<void>, invite, (const user_id &), (override));
};

TEST(Rooms, Instance) {
  mock_room_factory room_factory_;
  rooms test{room_factory_};
}

// TODO do a fixtur efor those tests

TEST(Rooms, Add) {
  mock_room_factory room_factory_;
  const room_id room_id_{"fun_id"};
  const room_name room_name_{"room_name"};
  const user_id user_id_{"user_id"};
  auto room_ = std::make_unique<mock_room>();
  EXPECT_CALL(*room_, get_room_id).WillRepeatedly(::testing::Return(room_id_));
  EXPECT_CALL(*room_, invite(user_id_))
      .WillOnce(
          ::testing::Return(::testing::ByMove(boost::make_ready_future())));
  room_ptr room_casted = std::move(room_);
  EXPECT_CALL(room_factory_, create(room_name_))
      .WillOnce(::testing::Return(
          ::testing::ByMove(boost::make_ready_future(std::move(room_casted)))));
  rooms test{room_factory_};
  auto add_future = test.get_or_create_room_id(room_name_, user_id_);
  EXPECT_EQ(add_future.get(), room_id_);
  EXPECT_EQ(test.get_room_count(), 1);
}

TEST(Rooms, AddTwoParticipants) {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  const room_name room_name_{"room_name"};
  const user_id user_id_{"user_id"};
  boost::promise<room_ptr> create_promise;
  EXPECT_CALL(room_factory_, create(room_name_))
      .WillOnce(
          ::testing::Return(::testing::ByMove(create_promise.get_future())));
  rooms test{room_factory_};
  int called{};
  for (int counter{}; counter < 2; ++counter) {
    test.get_or_create_room_id(room_name_, user_id_).then([&](auto result) {
      result.get();
      ++called;
    });
  }
  EXPECT_EQ(test.get_room_count(), 1);
  auto room_ = std::make_unique<mock_room>();
  EXPECT_CALL(*room_, get_room_id).WillRepeatedly(::testing::Return(room_id_));
  EXPECT_CALL(*room_, invite(user_id_))
      .Times(2)
      .WillOnce(
          ::testing::Return(::testing::ByMove(boost::make_ready_future())))
      .WillOnce(
          ::testing::Return(::testing::ByMove(boost::make_ready_future())));
  create_promise.set_value(std::move(room_));
  EXPECT_EQ(test.get_room_count(), 1);
}

MATCHER_P(HasCorrectError, message, "") { return arg.what() == message; }

TEST(Rooms, AddFail) {
  mock_room_factory room_factory_;
  std::string error_message = "failed";
  const room_name room_name_{"room_name"};
  const user_id user_id_{"user_id"};
  struct test_exception : virtual boost::exception, virtual std::exception {};
  EXPECT_CALL(room_factory_, create(room_name_))
      .WillOnce(::testing::Return(::testing::ByMove(
          boost::make_exceptional_future<room_ptr>(test_exception()))));
  rooms test{room_factory_};
  auto result = test.get_or_create_room_id(room_name_, user_id_);
  // test_exception does not work, because `enable_current_exception` not called
  EXPECT_THROW(result.get(), std::exception);
}

struct RoomWithParticipants : testing::Test {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  room_name room_name_{"room_name"};
  const user_id user_id_{"user_id"};
  rooms test{room_factory_};
  std::vector<boost::future<room_id>> participants;
  mock_room *room_;
  std::shared_ptr<int> room_alive_check;

  RoomWithParticipants() {
    auto room_ptr_ = std::make_unique<mock_room>();
    room_ = room_ptr_.get();
    room_alive_check = room_ptr_->alive_check;
    EXPECT_CALL(*room_, get_room_id)
        .WillRepeatedly(::testing::Return(room_id_));
    room_ptr room_casted = std::move(room_ptr_);
    EXPECT_CALL(room_factory_, create(room_name_))
        .WillOnce(::testing::Return(::testing::ByMove(
            boost::make_ready_future(std::move(room_casted)))));
    EXPECT_CALL(*room_, invite(::testing::_))
        .WillRepeatedly(::testing::Invoke([](auto user_id) {
          (void)user_id;
          return boost::make_ready_future();
        }));
  }
  void add_participants(int count) {
    for (int index = 0; index < count; ++index) {
      auto user_id_ = uuid::generate();
      participants.emplace_back(
          test.get_or_create_room_id(room_name_, user_id_));
    }
  }

  void remove_all_participants() { room_->on_empty(); }

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
