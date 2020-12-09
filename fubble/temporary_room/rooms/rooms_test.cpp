#include "rooms.hpp"
#include "uuid.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace temporary_room::rooms;

namespace {
class mock_room_factory : public room_factory {
public:
  MOCK_METHOD(void, create, (const std::string &), (override));
};

class mock_room : public room {
public:
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
  MOCK_METHOD(room_id, get_room_id, (), (const override));
  MOCK_METHOD(room_name, get_room_name, (), (const override));
  MOCK_METHOD(boost::future<void>, invite, (const user_id &), (override));
  MOCK_METHOD(bool, is_empty, (), (const override));
};

std::unique_ptr<mock_room> make_mock_room(room_id id, room_name name) {
  auto room_ptr_ = std::make_unique<mock_room>();
  ON_CALL(*room_ptr_, get_room_name()).WillByDefault(::testing::Return(name));
  EXPECT_CALL(*room_ptr_, get_room_id).WillRepeatedly(::testing::Return(id));
  EXPECT_CALL(*room_ptr_, invite(::testing::_))
      .WillRepeatedly([]([[maybe_unused]] auto user_id) {
        return boost::make_ready_future();
      });
  return room_ptr_;
}

struct Rooms : ::testing::Test {
  mock_room_factory room_factory_;
  room_id room_id_{"fun_id"};
  room_name room_name_{"room_name"};
  const user_id user_id_{"user_id"};
  rooms test{room_factory_};
};
} // namespace

TEST_F(Rooms, Instance) {}

TEST_F(Rooms, Add) {
  auto room_ = std::make_unique<mock_room>();
  ON_CALL(*room_, get_room_name()).WillByDefault(::testing::Return(room_name_));
  EXPECT_CALL(*room_, get_room_id).WillRepeatedly(::testing::Return(room_id_));
  EXPECT_CALL(*room_, invite(user_id_))
      .WillOnce(
          ::testing::Return(::testing::ByMove(boost::make_ready_future())));
  room_ptr room_casted = std::move(room_);
  EXPECT_CALL(room_factory_, create(room_name_)).WillOnce([&](auto) {
    room_factory_.on_room(room_casted);
  });
  auto add_future = test.get_or_create_room_id(room_name_, user_id_);
  EXPECT_EQ(add_future.get(), room_id_);
  EXPECT_EQ(test.get_room_count(), 1);
}

TEST_F(Rooms, AddTwoParticipants) {
  boost::promise<room_ptr> create_promise;
  EXPECT_CALL(room_factory_, create(room_name_)).WillOnce([&](auto) {
    auto room_ = std::make_unique<mock_room>();
    ON_CALL(*room_, get_room_name())
        .WillByDefault(::testing::Return(room_name_));
    EXPECT_CALL(*room_, get_room_id)
        .WillRepeatedly(::testing::Return(room_id_));
    EXPECT_CALL(*room_, invite(user_id_))
        .Times(2)
        .WillOnce(
            ::testing::Return(::testing::ByMove(boost::make_ready_future())))
        .WillOnce(
            ::testing::Return(::testing::ByMove(boost::make_ready_future())));
    room_ptr room_casted = std::move(room_);
    room_factory_.on_room(room_casted);
  });
  int called{};
  for (int counter{}; counter < 2; ++counter) {
    test.get_or_create_room_id(room_name_, user_id_).then([&](auto result) {
      result.get();
      ++called;
    });
  }
  EXPECT_EQ(test.get_room_count(), 1);
  EXPECT_EQ(test.get_room_count(), 1);
}

#if 0 // not implemented no more
MATCHER_P(HasCorrectError, message, "") { return arg.what() == message; }

TEST_F(Rooms, AddFail) {
  struct test_exception : virtual boost::exception, virtual std::exception {};
  EXPECT_CALL(room_factory_, create(room_name_))
      .WillOnce(::testing::Return(::testing::ByMove(
          boost::make_exceptional_future<room_ptr>(test_exception()))));
  auto result = test.get_or_create_room_id(room_name_, user_id_);
  // test_exception does not work, because `enable_current_exception` not called
  EXPECT_THROW(result.get(), std::exception);
}
#endif

namespace {
struct RoomWithParticipants : Rooms {
  std::vector<boost::future<room_id>> participants;
  mock_room *room_{};
  std::shared_ptr<int> room_alive_check;

  void expect_room_creation() {
    EXPECT_CALL(room_factory_, create(room_name_))
        .WillOnce([this](room_name name) {
          room_ptr room_casted =
              make_mock_room_and_set_to_fixture(room_id_, name);
          room_factory_.on_room(room_casted);
        });
  }

  room_ptr make_mock_room_and_set_to_fixture(room_id id, room_name name) {
    auto room_ptr_ = make_mock_room(id, name);
    room_ = room_ptr_.get();
    room_alive_check = room_ptr_->alive_check;
    room_ptr room_casted = std::move(room_ptr_);
    return room_casted;
  }

  void add_participants(int count) {
    for (int index = 0; index < count; ++index) {
      auto user_id_ = uuid::generate();
      participants.emplace_back(
          test.get_or_create_room_id(room_name_, user_id_));
    }
  }

  void remove_all_participants() {
    participants.clear();
    ASSERT_TRUE(room_);
    room_->on_empty();
  }

  void ensure_room_id_got_set() {
    for (auto &participant_ : participants)
      EXPECT_EQ(participant_.get(), room_id_);
  }
};
} // namespace

TEST_F(RoomWithParticipants, Remove) {
  expect_room_creation();
  add_participants(1);
  ensure_room_id_got_set();
  remove_all_participants();
  EXPECT_TRUE(room_alive_check.unique());
  EXPECT_EQ(test.get_room_count(), 0);
}

TEST_F(RoomWithParticipants, RemoveTwo) {
  expect_room_creation();
  add_participants(2);
  ensure_room_id_got_set();
  remove_all_participants();
  EXPECT_TRUE(room_alive_check.unique());
  EXPECT_EQ(test.get_room_count(), 0);
}

TEST_F(RoomWithParticipants, Renewal) {
  expect_room_creation();
  add_participants(1);
  remove_all_participants();
  EXPECT_CALL(room_factory_, create(room_name_))
      .WillOnce([this](room_name name) {
        room_ptr room_casted = make_mock_room("another_fun_id", name);
        room_factory_.on_room(room_casted);
      });
  add_participants(1);
}

TEST_F(RoomWithParticipants, Existing) {
  std::shared_ptr<room> room_ = make_mock_room(room_id_, room_name_);
  room_factory_.on_room(room_);
}

TEST_F(RoomWithParticipants, AddExisting) {
  room_ptr room_local = make_mock_room_and_set_to_fixture(room_id_, room_name_);
  room_factory_.on_room(room_local);
  add_participants(1);
  ensure_room_id_got_set();
  remove_all_participants();
  EXPECT_EQ(test.get_room_count(), 0);
}
