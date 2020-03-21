#include "rooms.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace temporary_room::rooms;

class mock_room_factory : public room_factory {
public:
  MOCK_METHOD(boost::future<room_id>, create, (), (override));
  MOCK_METHOD(void, destroy, (const room_id &), (override));
};

class mock_participant : public participant {
public:
  MOCK_METHOD(void, set_room, (const room_id &), (override));
  MOCK_METHOD(void, set_error, (const std::runtime_error &), (override));
};

TEST(Rooms, Instance) {
  mock_room_factory room_factory_;
  rooms test{room_factory_};
}

TEST(Rooms, Add) {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  EXPECT_CALL(room_factory_, create)
      .WillOnce(::testing::Return(
          ::testing::ByMove(boost::make_ready_future<room_id>(room_id_))));
  mock_participant participant_;
  EXPECT_CALL(participant_, set_room(room_id_));
  rooms test{room_factory_};
  test.add_participant(participant_, "name");
  EXPECT_EQ(test.get_room_count(), 1);
}

TEST(Rooms, AddTwoParticipants) {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  boost::promise<room_id> create_promise;
  EXPECT_CALL(room_factory_, create)
      .WillOnce(
          ::testing::Return(::testing::ByMove(create_promise.get_future())));
  std::array<mock_participant, 2> participants;
  rooms test{room_factory_};
  for (auto &participant_ : participants) {
    EXPECT_CALL(participant_, set_room(room_id_));
    test.add_participant(participant_, "name");
  }
  EXPECT_EQ(test.get_room_count(), 1);
  create_promise.set_value(room_id_);
  EXPECT_EQ(test.get_room_count(), 1);
}

MATCHER_P(HasCorrectError, message, "") { return arg.what() == message; }

TEST(Rooms, AddFail) {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  std::string error_message = "failed";
  EXPECT_CALL(room_factory_, create)
      .WillOnce(::testing::Return(
          ::testing::ByMove(boost::make_exceptional_future<room_id>(
              std::runtime_error(error_message)))));
  mock_participant participant_;
  EXPECT_CALL(participant_, set_error(HasCorrectError(error_message)));
  rooms test{room_factory_};
  test.add_participant(participant_, "name");
}

struct RoomWithParticipants : testing::Test {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  room_name room_name_{"room_name"};
  rooms test{room_factory_};
  std::vector<std::shared_ptr<mock_participant>> participants;

  RoomWithParticipants() {
    EXPECT_CALL(room_factory_, create)
        .WillOnce(::testing::Return(
            ::testing::ByMove(boost::make_ready_future<room_id>(room_id_))));
  }
  void add_participants(int count) {
    for (int index = 0; index < count; ++index) {
      auto participant_ = std::make_shared<mock_participant>();
      participants.push_back(participant_);
      EXPECT_CALL(*participant_, set_room(room_id_));
      test.add_participant(*participant_, room_name_);
    }
  }
  void remove_all_participants() {
    for (auto &participant_ : participants)
      test.remove_participant(*participant_, room_name_);
  }
};

TEST_F(RoomWithParticipants, Remove) {
  add_participants(1);
  EXPECT_CALL(room_factory_, destroy(room_id_));
  remove_all_participants();
  EXPECT_EQ(test.get_room_count(), 0);
}

TEST_F(RoomWithParticipants, RemoveTwo) {
  add_participants(2);
  EXPECT_CALL(room_factory_, destroy(room_id_));
  remove_all_participants();
  EXPECT_EQ(test.get_room_count(), 0);
}
