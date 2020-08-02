#include "events.hpp"
#include "fixture.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

struct Events : fixture {};

TEST_F(Events, Create) {
  auto client_ = create_registered_client();
  EXPECT_TRUE(matrix::event_parser::create(client_->get_users()));
  auto all_events = matrix::events::create_all_events();
  EXPECT_TRUE(all_events);
  EXPECT_TRUE(matrix::events::create_room_filter(*all_events, "room_id"));
}

TEST_F(Events, MRoomMessage) {
  auto client_ = create_registered_client();
  auto parser = matrix::event_parser::create(client_->get_users());
  // https://matrix.org/docs/spec/client_server/latest#m-text
  const std::string test_json = R"(
  {
      "content": {
          "body": "This is an example text message",
          "format": "org.matrix.custom.html",
          "formatted_body": "<b>This is an example text message</b>",
          "msgtype": "m.text"
      },
      "event_id": "$143273582443PhrSn:example.org",
      "origin_server_ts": 1432735824653,
      "room_id": "!jEsUZKDJdhlrceRyVU:example.org",
      "sender": "@example:example.org",
      "type": "m.room.message",
      "unsigned": {
          "age": 1234
      }
  }
  )";
  auto result = parser->parse(nlohmann::json::parse(test_json));
  EXPECT_TRUE(result);
  auto room_event = dynamic_cast<matrix::event::room_event *>(result.get());
  EXPECT_TRUE(result);
  EXPECT_EQ(room_event->room_id, "!jEsUZKDJdhlrceRyVU:example.org");
}
