#ifndef UUID_3978FC5A_1DD5_4819_B7E0_27C086D18F0D
#define UUID_3978FC5A_1DD5_4819_B7E0_27C086D18F0D

#include "temporary_room/logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json.hpp>

namespace temporary_room::net::server {
// TODO add http result code
using response_future = boost::future<nlohmann::json>;
using request_type =
    boost::beast::http::request<boost::beast::http::string_body>;
using response_type =
    boost::beast::http::response<boost::beast::http::string_body>;
class connection {
public:
  connection(boost::asio::ip::tcp::socket &&socket);
  boost::future<void> do_();

  std::function<response_future(const std::string &target,
                                const nlohmann::json &request)>
      on_request;

protected:
  bool check_error(const boost::system::error_code error);
  void read_next_request();
  void on_got_request(const request_type &request);
  void setup_response();
  void send_error(const std::exception &error);
  void on_got_response(response_future &response_future_);
  void send_response();

  temporary_room::logger logger{"net::server::connection"};
  std::shared_ptr<boost::promise<void>> promise =
      std::make_shared<boost::promise<void>>();
  boost::beast::tcp_stream stream;
  boost::beast::flat_buffer buffer;
  std::shared_ptr<request_type> request;
  std::shared_ptr<response_type> response;
};

class acceptor {
public:
  struct config {
    unsigned short port;
  };
  acceptor(boost::asio::io_context &context, const config &config_);

  void listen();
  boost::future<void> run();
  void stop();

  unsigned short get_port() const;

  std::function<response_future(const std::string &target,
                                const nlohmann::json &request)>
      on_request;

protected:
  void accept_next();
  void do_session();

  temporary_room::logger logger{"net::server"};
  boost::inline_executor executor;
  boost::asio::io_context &context;
  boost::asio::ip::tcp::acceptor acceptor_{context};
  const config config_;
  std::shared_ptr<boost::promise<void>> run_promise;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket;
  bool listening{};
};
} // namespace temporary_room::net::server

#endif
