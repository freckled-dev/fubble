#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

namespace {
logging::logger logger{"matrix_test"};
namespace http {
const std::string server = "localhost";
const std::string port = "8008";
const std::string host = server + ":" + port;
const std::string target_prefix = "/_matrix/client/r0/";
#if 1
const std::string agent = BOOST_BEAST_VERSION_STRING;
#else
const std::string agent =
    "curl";       // with curl matrix returns a pretty-printed json
#endif
#if 1
int version = 11; // http 1.1
#else
int version = 10; // http 1.0
#endif
struct client {
  boost::asio::io_context &context;
  boost::beast::tcp_stream stream{context};
  std::optional<std::string> auth_token;

  client(boost::asio::io_context &context) : context(context) {
    boost::asio::ip::tcp::resolver resolver{context};
    const auto resolved = resolver.resolve(http::server, http::port);
    stream.connect(resolved);
  }
  ~client() {
    boost::system::error_code error;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                             error);
    if (!error)
      return;
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "an error occurred, closing the websocket stream, error:"
        << error.message();
    if (error == boost::beast::errc::not_connected)
      return;
    EXPECT_TRUE(false);
  }

  using response_type =
      boost::beast::http::response<boost::beast::http::string_body>;
  using request_type =
      boost::beast::http::request<boost::beast::http::string_body>;

  response_type read_response() {
    boost::beast::flat_buffer buffer;
    response_type response;
    boost::beast::http::read(stream, buffer, response);
    BOOST_LOG_SEV(logger, logging::severity::info) << "response:" << response;
    return response;
  }

  response_type get(std::string target) {
    boost::beast::http::request<boost::beast::http::string_body> request{
        boost::beast::http::verb::get, target, http::version};
    set_default_request_headers(request);
    BOOST_LOG_SEV(logger, logging::severity::info) << "get request:" << request;
    boost::beast::http::write(stream, request);
    return read_response();
  }

  response_type post(std::string target, const nlohmann::json &message) {
    request_type request{boost::beast::http::verb::post, target, http::version};
    set_default_request_headers(request);
    request.body() = message.dump();
    request.prepare_payload();
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "post request:" << request;
    boost::beast::http::write(stream, request);
    return read_response();
  }

  void set_default_request_headers(request_type &set) {
    set.set(boost::beast::http::field::host, http::host);
    set.set(boost::beast::http::field::user_agent, http::agent);
    set.set(boost::beast::http::field::accept, "application/json");
    if (auth_token)
      set.set(boost::beast::http::field::authorization,
              std::string("Bearer ") + auth_token.value());
  }
};
} // namespace http
// TODO move into namespace client and rename to client
} // namespace

TEST(Matrix, Versions) {
  boost::asio::io_context context;
  http::client client{context};
  const std::string target{"/_matrix/client/versions"};
  auto response = client.get(target);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  const std::string body = response.body();
  nlohmann::json verify = nlohmann::json::parse(body);
  auto versions = verify["versions"];
  EXPECT_EQ(versions.type(), nlohmann::json::value_t::array);
  EXPECT_FALSE(versions.empty());
  BOOST_LOG_SEV(logger, logging::severity::info) << "response_body:" << body;
}

namespace {
void check_valid_registration(const nlohmann::json &check) {
  EXPECT_TRUE(check.contains("user_id"));
  EXPECT_TRUE(check.contains("access_token"));
  EXPECT_TRUE(check.contains("device_id"));
}
nlohmann::json register_as_guest(http::client &client) {
  const std::string target = http::target_prefix + "register?kind=guest";
  auto register_ = nlohmann::json::object();
  register_["initial_device_display_name"] = "A fun guest name";
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  auto response = client.post(target, register_);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  auto check = nlohmann::json::parse(response.body());
  check_valid_registration(check);
  return check;
}
nlohmann::json register_as_user(http::client &client) {
  const std::string target = http::target_prefix + "register";
  auto register_ = nlohmann::json::object();
  register_["password"] = ""; // mandatory if not guest
  register_["initial_device_display_name"] = "A fun user name";
  auto auth = nlohmann::json::object();
  auth["type"] = "m.login.dummy";
  register_["auth"] = auth;
  auto response = client.post(target, register_);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  auto check = nlohmann::json::parse(response.body());
  check_valid_registration(check);
  return check;
}
} // namespace

// https://matrix.org/docs/spec/client_server/latest#account-registration-and-management
TEST(Matrix, RegistrationGuest) {
  boost::asio::io_context context;
  http::client client{context};
  register_as_guest(client);
}

TEST(Matrix, RegistrationUser) {
  boost::asio::io_context context;
  http::client client{context};
  register_as_user(client);
}

namespace {
struct client {
  boost::asio::io_context &context;
  http::client http_client{context};
  std::string user_id;

  client(boost::asio::io_context &context) : context(context) {
    auto response = register_as_user(http_client);
    user_id = response["user_id"];
    auto token = response["access_token"];
    http_client.auth_token = token;
  }
};
std::string create_room(client &client_) {
  http::client &http_client = client_.http_client;
  auto create = nlohmann::json::object();
  auto response = http_client.post(http::target_prefix + "createRoom", create);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  auto response_json = nlohmann::json::parse(response.body());
  EXPECT_TRUE(response_json.contains("room_id"));
  return response_json["room_id"];
}
} // namespace

TEST(Matrix, CreateRoom) {
  boost::asio::io_context context;
  client client_{context};
  EXPECT_FALSE(create_room(client_).empty());
}

TEST(Matrix, CreateRoomWithParameter) {
  boost::asio::io_context context;
  client client_{context};
  http::client &http_client = client_.http_client;

  auto create = nlohmann::json::object();
#if 0
  create["room_alias_name"] = "fun";
#endif
  auto creation_content = nlohmann::json::object();
  creation_content["m.federate"] = false;
  create["creation_content"] = creation_content;
  auto response = http_client.post(http::target_prefix + "createRoom", create);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  auto response_json = nlohmann::json::parse(response.body());
  EXPECT_TRUE(response_json.contains("room_id"));
}

namespace {
void join_room_by_id(client &joiner, std::string room_id) {
  http::client &http_join = joiner.http_client;
  auto join_path = http::target_prefix + "rooms/" + room_id + "/join";
  auto join_request = nlohmann::json::object();
  auto response = http_join.post(join_path, join_request);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  auto response_json = nlohmann::json::parse(response.body());
  EXPECT_TRUE(response_json.contains("room_id"));
  EXPECT_EQ(response_json["room_id"], room_id);
}
} // namespace

TEST(Matrix, JoinRoom) {
  boost::asio::io_context context;
  client client_creator{context};
  auto room_id = create_room(client_creator);
  client client_join{context};
  join_room_by_id(client_join, room_id);
}

namespace {
struct sync_result {
  nlohmann::json repsonse;
  std::string next_batch;
};
sync_result sync(client &client_, std::optional<std::string> next_batch) {
  http::client &http_creator = client_.http_client;
  auto sync_path = http::target_prefix + "sync";
  if (next_batch)
    sync_path += "?since=" + next_batch.value();
#if 0
  int timeout_ms = 1000;
  sync_path += "&timeout=" + std::to_string(timeout_ms);
#endif
  auto response = http_creator.get(sync_path);
  EXPECT_EQ(response.result(), boost::beast::http::status::ok);
  sync_result result;
  result.repsonse = nlohmann::json::parse(response.body());
  EXPECT_TRUE(result.repsonse.contains("next_batch"));
  result.next_batch = result.repsonse["next_batch"];
  return result;
}
} // namespace

TEST(Matrix, Sync) {
  boost::asio::io_context context;
  client client_{context};
  auto room_id = create_room(client_);
  auto response = sync(client_, {});
  EXPECT_FALSE(response.next_batch.empty());
}

TEST(Matrix, SyncJoin) {
  boost::asio::io_context context;
  client client_create{context};
  auto room_id = create_room(client_create);
  auto sync_response = sync(client_create, {});
  client client_join{context};
  join_room_by_id(client_join, room_id);
  sync_response = sync(client_create, sync_response.next_batch);
  auto join_events = sync_response.repsonse["rooms"]["join"];
  EXPECT_TRUE(join_events.contains(room_id));
  auto room_timeline_events = join_events[room_id]["timeline"]["events"];
  EXPECT_EQ(room_timeline_events.size(), std::size_t{1});
  auto event = room_timeline_events[0];
  std::string event_type = event["type"];
  EXPECT_EQ(event_type, "m.room.member");
  std::string event_sender = event["sender"];
  EXPECT_EQ(event_sender, client_join.user_id);
}

TEST(Matrix, Invite) {
  boost::asio::io_context context;
  client client_create{context};
  auto room_id = create_room(client_create);
  client client_join{context};
  sync(client_join, {});
}
