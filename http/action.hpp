#ifndef UUID_D6B7D20F_925D_4C0E_97C3_937A82E46930
#define UUID_D6B7D20F_925D_4C0E_97C3_937A82E46930

#include "fields.hpp"
#include "logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json.hpp>

namespace http {
class error_not_status_200 : public virtual boost::exception,
                             public virtual std::exception {
  std::string message;

public:
  error_not_status_200(boost::beast::http::status status) {
    message = "status: " + std::to_string(static_cast<int>(status));
  };
  const char *what() const noexcept override { return message.c_str(); }
};
class action {
public:
  action(boost::asio::io_context &context, boost::beast::http::verb verb,
         const std::string &target, const server &server_,
         const fields &fields_);

  void set_request_body(const nlohmann::json &body);
  using async_result = std::pair<boost::beast::http::status, nlohmann::json>;
  using async_result_future = boost::future<async_result>;
  using async_result_promise = boost::promise<async_result>;
  async_result_future do_();

protected:
  void
  on_resolved(const boost::system::error_code &error,
              const boost::asio::ip::tcp::resolver::results_type &resolved);
  void on_connected(const boost::system::error_code &error);
  void do_request();
  void on_request_send(const boost::system::error_code &error);
  void read_response();
  void on_response_read(const boost::system::error_code &error);
  bool check_and_handle_error(const boost::system::error_code &error);

  http::logger logger{"action"};
  boost::beast::tcp_stream stream;
  boost::asio::ip::tcp::resolver resolver;
  const server server_;
  using response_type =
      boost::beast::http::response<boost::beast::http::string_body>;
  using request_type =
      boost::beast::http::request<boost::beast::http::string_body>;
  std::shared_ptr<async_result_promise> promise;
  request_type request;
  response_type response;
  boost::beast::flat_buffer response_buffer;
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
};
} // namespace http

#endif
