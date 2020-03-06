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
}

// TODO test for failed room creation
