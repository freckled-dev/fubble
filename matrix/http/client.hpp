#ifndef UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7
#define UUID_C10B4395_33FA_4EC1_9CBD_6A81C67FD8D7

#include "logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json.hpp>

namespace matrix::http {
class client {
public:
  struct server {
    std::string server;
    std::string port;
  };
  struct fields {
    fields() = default;
    fields(const server &server) { host = server.server + ":" + server.port; }
    std::string host;
    std::string target_prefix{"/_matrix/client/r0/"};
    std::string agent = BOOST_BEAST_VERSION_STRING;
    int version = 11; // http 1.1
    std::optional<std::string> auth_token;
  };
  client(boost::asio::io_context &context, const server &server_,
         const fields &fields_);
  ~client();

  using async_result = std::pair<boost::beast::http::status, nlohmann::json>;
  using async_result_future = boost::future<async_result>;
  using async_result_promise = boost::promise<async_result>;
  // TODO refactor. shall not do one get/post at a time
  async_result_future get(const std::string &target);
  async_result_future post(const std::string &target,
                           const nlohmann::json &content);
  async_result_future put(const std::string &target,
                          const nlohmann::json &content);

protected:
  void create_promise();
  void clear_promise_and_action();
  void connect_and_do_action();
  void do_action();
  void
  on_resolved(const boost::system::error_code &error,
              const boost::asio::ip::tcp::resolver::results_type &resolved);
  void on_connected(const boost::system::error_code &error);
  bool check_and_handle_error(const boost::system::error_code &error);
  void do_request();
  void on_request_send(const boost::system::error_code &error);
  void read_response();
  void on_response_read(const boost::system::error_code &error);

  matrix::logger logger{"http::client"};
  boost::beast::tcp_stream stream;
  bool connected{};
  boost::asio::ip::tcp::resolver resolver;
  const server server_;
  const fields fields_;
  std::unique_ptr<async_result_promise> promise;
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
  struct action {
    std::string target;

    action(const std::string &target) : target(target) {}
    virtual ~action() = default;
    virtual void do_(client &self) = 0;
  };
  struct get_action : action {
    using action::action;
    void do_(client &self) override;
  };
  struct post_action : action {
    nlohmann::json content;
    post_action(const std::string &target, const nlohmann::json &content)
        : action(target), content(content) {}
    void do_(client &self) override;
  };
  std::unique_ptr<action> current_action;
  using response_type =
      boost::beast::http::response<boost::beast::http::string_body>;
  using request_type =
      boost::beast::http::request<boost::beast::http::string_body>;
  request_type request;
  response_type response;
  boost::beast::flat_buffer response_buffer;
};
} // namespace matrix::http

#endif
