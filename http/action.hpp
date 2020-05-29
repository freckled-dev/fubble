#ifndef UUID_D6B7D20F_925D_4C0E_97C3_937A82E46930
#define UUID_D6B7D20F_925D_4C0E_97C3_937A82E46930

#include "fields.hpp"
#include "logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json_fwd.hpp>

namespace http {
class connection_creator;
class connection;
class connector;
class action {
public:
  action(connection_creator &creator, boost::beast::http::verb verb,
         const std::string &target, const server &server_,
         const fields &fields_);
  ~action();

  void set_request_body(const nlohmann::json &body);
  using async_result = std::pair<boost::beast::http::status, nlohmann::json>;
  using async_result_future = boost::future<async_result>;
  using async_result_promise = boost::promise<async_result>;
  async_result_future do_();

  void cancel();

protected:
  void send_request();
  void on_request_send(const boost::system::error_code &error);
  void read_response();
  void on_response_read(const boost::system::error_code &error);
  bool check_and_handle_error(const boost::system::error_code &error);

  http::logger logger{"action"};
  boost::inline_executor executor;
  connection_creator &connection_creator_;
  const server server_;
  const boost::beast::http::verb verb;
  const std::string target;
  const fields fields_;
  std::shared_ptr<async_result_promise> promise;
  std::unique_ptr<connection> connection_;
  std::unique_ptr<connector> connector_;

  using response_type =
      boost::beast::http::response<boost::beast::http::string_body>;
  using request_type =
      boost::beast::http::request<boost::beast::http::string_body>;
  struct buffers {
    request_type request;
    response_type response;
    boost::beast::flat_buffer response_buffer;
  };
  std::shared_ptr<buffers> buffers_ = std::make_shared<buffers>();
  std::shared_ptr<int> alive_check =
      std::make_shared<int>(42); // TODO use the promise?
};
} // namespace http

#endif
