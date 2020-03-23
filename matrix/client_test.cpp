#include "authentification.hpp"
#include "client_factory.hpp"
#include "room.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

using namespace matrix;

struct Client : ::testing::Test {
  boost::inline_executor executor;
  boost::asio::io_context context;
  http::client::server server_information{"localhost", "8008"};
  http::client::fields fields_information{server_information};
  http::client_factory http_client_factory{context, server_information,
                                           fields_information};
  room_factory room_factory_;
  client_factory client_factory_{room_factory_, http_client_factory};
  authentification authentification_{http_client_factory, client_factory_};
};

TEST_F(Client, Instance) {}

TEST_F(Client, Fun) {
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
                    called = true;
                    context.stop();
                  });
  context.run();
  EXPECT_TRUE(called);
  done.get();
}
