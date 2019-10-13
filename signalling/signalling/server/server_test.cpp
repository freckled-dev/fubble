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
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>

static auto make_injector(boost::asio::io_context &context) {
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
  signalling::client::client client_ =
      injector.create<signalling::client::client>();
  signalling::client::client client_answering =
      injector.create<signalling::client::client>();

  void connect(signalling::client::client &client) const {
    auto service = std::to_string(acceptor.get_port());
    client("localhost", service, session_key);
  }
  void close() {
    if (client_.get_connection())
      client_.close();
    if (client_answering.get_connection())
      client_answering.close();
    try {
      server_.close();
    } catch (...) {
      // ignore already closed
    }
  }
};

TEST_F(Server, SetUp) {
  server_.close();
  context.run();
}

TEST_F(Server, SingleConnect) {
  bool connected{};
  client_.on_connected.connect([&] {
    auto connection = client_.get_connection();
    EXPECT_TRUE(connection);
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
  client_.on_connected.connect([&] {
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
  client_.on_connected.connect(connected_callback);
  client_answering.on_connected.connect(connected_callback);
  connect(client_);
  connect(client_answering);
  context.run();
  EXPECT_EQ(connected, 2);
}

TEST_F(Server, CantConnect) {
  server_.close();
  bool called{};
  client_.on_error.connect([&](auto error) {
    called = true;
    EXPECT_EQ(error.code(), boost::asio::error::connection_refused);
  });
  client_("localhost", "", session_key);
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, StateOffering) {
  bool called{};
  client_.on_create_offer.connect([&] {
    called = true;
    close();
  });
  client_("localhost", std::to_string(acceptor.get_port()), session_key);
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, StateAnswering) {
  connect(client_);
  connect(client_answering);
  bool called{};
  client_answering.on_create_answer.connect([&] {
    EXPECT_FALSE(called);
    called = true;
    close();
  });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, SendReceiveOffer) {
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
  connect(client_);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_.on_answer.connect([&](const auto answer_) {
    EXPECT_FALSE(called);
    called = true;
    EXPECT_EQ(answer_.sdp, sdp);
    close();
  });
  client_answering.on_create_answer.connect(
      [&] { client_answering.send_answer({sdp}); });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, SendReceiveIceCandidate) {
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
  client_.on_create_offer.connect([&] { client_.send_ice_candidate({sdp}); });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Server, Close) {
  connect(client_);
  connect(client_answering);
  bool called{};
  const std::string sdp = "fun sdp";
  client_.on_closed.connect([&]() {
    EXPECT_FALSE(called);
    called = true;
    server_.close();
  });
  client_answering.on_create_answer.connect([&] {
    // sadad
    client_answering.close();
  });
  context.run();
  EXPECT_TRUE(called);
}
