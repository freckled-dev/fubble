#ifndef UUID_810E6130_85A4_482A_A5E4_EFE1E19E66B7
#define UUID_810E6130_85A4_482A_A5E4_EFE1E19E66B7

#include "connection.hpp"
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/variant.hpp>
#include <queue>

namespace websocket {
class connection_impl : public connection {
public:
  connection_impl(boost::asio::io_context &context, bool secure);
  ~connection_impl();
  boost::future<void> send(const std::string &message) override;
  boost::future<std::string> read() override;
  boost::future<void> close() override;

  using http_stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  using ssl_stream_type =
      boost::beast::ssl_stream<boost::asio::ip::tcp::socket>;
  using https_stream_type = boost::beast::websocket::stream<ssl_stream_type>;
  using stream_type = boost::variant<http_stream_type, https_stream_type>;
  stream_type &get_native();
  boost::asio::ssl::context &get_ssl_context();

private:
  void send_next_from_queue();
  void on_send(const boost::system::error_code &error, std::size_t);
  void on_read(const boost::system::error_code &error, std::size_t);

  websocket::logger logger{"connection_impl"};
  boost::asio::ssl::context ssl_context;
  std::shared_ptr<stream_type> stream;
  std::shared_ptr<boost::beast::flat_buffer> read_buffer =
      std::make_shared<boost::beast::flat_buffer>();

  std::shared_ptr<boost::promise<std::string>> read_promise;
  struct send_item {
    std::string message;
    boost::promise<void> completion;
  };
  std::shared_ptr<std::queue<send_item>> send_queue =
      std::make_shared<std::queue<send_item>>();
  bool sending{};
  bool reading{};
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
};
} // namespace websocket

#endif
