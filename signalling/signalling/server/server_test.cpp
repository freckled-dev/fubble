#include "boost_di_extension_scopes_session.hpp"
#include "connection_creator.hpp"
#include "executor_asio.hpp"
#include "server.hpp"
#include "signalling/client/client.hpp"
#include "signalling/client/connection.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/device/creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/registration_handler.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>

namespace {
auto make_injector(boost::asio::io_context &context) {
  namespace di = boost::di;
  using executor_type = boost::executor_adaptor<executor_asio>;
  return di::make_injector<di::extension::shared_config>(
      di::bind<boost::asio::io_context>.to(context),
      di::bind<boost::executor>.to(std::make_shared<executor_type>(context)));
}

struct Server : testing::Test {
  const std::string session_key = "fun session key";
  boost::asio::io_context context;
  decltype(make_injector(context)) injector = make_injector(context);
  boost::executor_adaptor<executor_asio> executor{context};
  websocket::acceptor &acceptor = injector.create<websocket::acceptor &>();
  signalling::server::server &server_ =
      injector.create<signalling::server::server &>();
  websocket::connector_creator &websocket_connector =
      injector.create<websocket::connector_creator &>();
  signalling::client::connection_creator &connection_creator =
      injector.create<signalling::client::connection_creator &>();
  signalling::client::client client_{websocket_connector, connection_creator};
  signalling::client::client client_answering{websocket_connector,
                                              connection_creator};

  void connect(signalling::client::client &client) const {
    auto service = std::to_string(acceptor.get_port());
    client.set_connect_information({"localhost", service, "/"});
    client.connect(session_key);
  }
  static void shall_want_to_negotiate(signalling::client::client &client) {
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
    [[maybe_unused]] auto &connection = client_.get_connection();
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
  client_.set_connect_information({"localhost", "", "/"});
  client_.connect(session_key);
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
