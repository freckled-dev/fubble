#include "connection_creator.hpp"
#include "executor_asio.hpp"
#include "http/connection_creator.hpp"
#include "server.hpp"
#include "signaling/client/client.hpp"
#include "signaling/client/connection.hpp"
#include "signaling/client/connection_creator.hpp"
#include "signaling/device/creator.hpp"
#include "signaling/json_message.hpp"
#include "signaling/registration_handler.hpp"
#include "utils/uuid.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>

namespace {

struct Server : testing::Test {
  const std::string session_key = "fun session key";
  boost::asio::io_context context;
  boost::executor_adaptor<executor_asio> executor{context};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::acceptor acceptor{context, websocket_connection_creator,
                               websocket::acceptor::config{}};
  signaling::json_message signaling_json;
  signaling::server::connection_creator server_connection_creator{
      executor, signaling_json};
  signaling::device::creator device_creator_{executor};
  signaling::registration_handler registration_handler{device_creator_};
  signaling::server::server server_{
      executor, acceptor, server_connection_creator, registration_handler};

  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};
  signaling::client::connection_creator connection_creator{context, executor,
                                                            signaling_json};
  std::unique_ptr<signaling::client::client> client_instance =
      signaling::client::client::create(websocket_connector,
                                         connection_creator);
  signaling::client::client &client_{*client_instance};
  std::unique_ptr<signaling::client::client> client_answering_instance =
      signaling::client::client::create(websocket_connector,
                                         connection_creator);
  signaling::client::client &client_answering{*client_answering_instance};

  std::unique_ptr<signaling::client::client>
  create_client_and_connect(const std::string &token = uuid::generate()) {
    std::unique_ptr<signaling::client::client> result =
        signaling::client::client::create(websocket_connector,
                                           connection_creator);
    connect(*result, token);
    return result;
  }
  void connect(signaling::client::client &client,
               const std::string token = uuid::generate()) const {
    auto service = std::to_string(acceptor.get_port());
    client.set_connect_information({false, "localhost", service, "/"});
    client.connect(token, session_key);
  }
  static void shall_want_to_negotiate(signaling::client::client &client) {
    client.on_registered.connect([&] { client.send_want_to_negotiate(); });
  }
  void close() {
    client_.close();
    client_answering.close();
    try {
      server_.close();
    } catch (...) {
      // ignore already closed
    }
  }
};
} // namespace

TEST_F(Server, SetUp) {
  server_.close();
  context.run();
}

TEST_F(Server, SingleConnect) {
  bool connected{};
  client_.on_registered.connect([&] {
    EXPECT_FALSE(connected);
    connected = true;
    close();
  });
  connect(client_);
  context.run();
  EXPECT_TRUE(connected);
}

TEST_F(Server, ClientClose) {
  bool connected{};
  client_.on_registered.connect([&] {
    EXPECT_FALSE(connected);
    connected = true;
    close();
  });
  connect(client_);
  context.run();
  EXPECT_TRUE(connected);
}

TEST_F(Server, DoubleConnect) {
  int connected{};
  auto connected_callback = [&] {
    ++connected;
    if (connected != 2)
      return;
    close();
  };
  client_.on_registered.connect(connected_callback);
  client_answering.on_registered.connect(connected_callback);
  connect(client_);
  connect(client_answering);
  context.run();
  EXPECT_EQ(connected, 2);
}

TEST_F(Server, CantConnect) {
  server_.close();
  bool called{};
  client_.on_error.connect([&](auto error) {
    (void)error;
    called = true;
  });
  client_.set_connect_information({false, "localhost", "", "/"});
  client_.connect("token", session_key);
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, StateCreateOffer) {
  bool called{};
  client_.on_create_offer.connect([&] {
    EXPECT_FALSE(called);
    called = true;
    close();
  });
  shall_want_to_negotiate(client_);
  connect(client_);
  connect(client_answering);
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, SendReceiveOffer) {
  shall_want_to_negotiate(client_);
  connect(client_);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_answering.on_offer.connect([&](const auto offer) {
    EXPECT_FALSE(called);
    called = true;
    EXPECT_EQ(offer.sdp, sdp);
    close();
  });
  client_.on_create_offer.connect([&] { client_.send_offer({sdp}); });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, SendReceiveAnswer) {
  shall_want_to_negotiate(client_);
  connect(client_);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_.on_create_offer.connect([&] { client_.send_offer({sdp}); });
  client_.on_answer.connect([&](const auto answer_) {
    EXPECT_FALSE(called);
    called = true;
    EXPECT_EQ(answer_.sdp, sdp);
    close();
  });
  client_answering.on_offer.connect(
      [&](auto) { client_answering.send_answer({sdp}); });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, SendReceiveIceCandidate) {
  shall_want_to_negotiate(client_);
  connect(client_);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_answering.on_ice_candidate.connect([&](const auto candidate) {
    EXPECT_FALSE(called);
    called = true;
    EXPECT_EQ(candidate.sdp, sdp);
    close();
  });
  client_.on_create_offer.connect([&] {
    client_.send_ice_candidate({0, "mline", sdp});
  });
  context.run();
  EXPECT_TRUE(called);
}

#if 0 // changed use case. when a connection ends the server does not shutdown
      // the partner connection. not sure yet if the close logic shall happen
      // per signaling. maybe do a shutdown message. or use websockets shutdown?
TEST_F(Server, Close) {
  connect(client_);
  shall_want_to_negotiate(client_answering);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_.on_closed.connect([&]() {
    EXPECT_FALSE(called);
    called = true;
    server_.close();
  });
  client_answering.on_create_offer.connect([&] { client_answering.close(); });
  context.run();
  EXPECT_TRUE(called);
}
#endif

TEST_F(Server, Reconnect) {
  shall_want_to_negotiate(client_);
  const std::string token = uuid::generate();
  auto first = create_client_and_connect(token);
  bool called{};
  std::unique_ptr<signaling::client::client> second;
  auto on_second_registered = [&] {
    called = true;
    EXPECT_FALSE(registration_handler.get_registered().empty());
    EXPECT_TRUE(registration_handler.get_registered()[0].devices[0]);
    second->close();
    server_.close();
  };
  first->on_registered.connect([&] {
    second = create_client_and_connect(token);
    second->on_registered.connect(on_second_registered);
  });
  context.run();
  EXPECT_TRUE(called);
}
