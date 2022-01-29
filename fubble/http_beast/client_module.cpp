#include "client_module.hpp"
#include <boost/asio/dispatch.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <nlohmann/json.hpp>
#include <system_error>
#include <thread>

using namespace fubble::http_beast;
using namespace fubble;

namespace {

using tcp = boost::asio::ip::tcp;

class request_thread_safe
    : public std::enable_shared_from_this<request_thread_safe>,
      public http2::request {
public:
  request_thread_safe(std::shared_ptr<boost::asio::io_context> io_context,
                      http2::server server, boost::beast::http::verb verb,
                      std::string target)
      : io_context{io_context}, server{server}, verb{verb}, target{target} {}

  ~request_thread_safe() {
    if (run_thread.joinable())
      run_thread.join();
  }

  // TODO port to real async
  void
  async_run(std::function<void(http2::response_result)> callback_) override {
    BOOST_ASSERT(callback_);
    run_thread = std::thread{[this, callback_, thiz = shared_from_this()]() {
      auto result = run();
      // switch back to main thread
      boost::asio::dispatch(*io_context, [result = std::move(result), callback_,
                                          thiz = shared_from_this()]() mutable {
        callback_(std::move(result));
      });
    }};
  }

  std::error_code check_boost_error_and_cancelled(
      const boost::system::error_code &error) const {
    if (error)
      return error;
    if (cancelled)
      return http2::error_code::cancelled;
    return std::error_code{};
  }

  http2::response_result run() override {
    // in the async variant the class may get destructed while `run` is getting
    // called. so keep local copies.
    tcp::resolver resolver(*io_context);
    boost::beast::tcp_stream stream(*io_context);
    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12};
    using ssl_stream_type =
        boost::beast::ssl_stream<boost::beast::tcp_stream &>;
    boost::beast::error_code error;
    auto const results = resolver.resolve(server.host, server.service, error);
    if (auto check = check_boost_error_and_cancelled(error); check)
      return check;
    stream.connect(results, error);
    if (auto check = check_boost_error_and_cancelled(error); check)
      return check;
    std::unique_ptr<ssl_stream_type> ssl_stream;
    if (server.secure)
      ssl_stream = std::make_unique<ssl_stream_type>(stream, context);
    if (ssl_stream) {
      if (!SSL_set_tlsext_host_name(ssl_stream->native_handle(),
                                    server.host.c_str())) {
        return boost::beast::error_code{static_cast<int>(::ERR_get_error()),
                                        boost::asio::error::get_ssl_category()};
      }
      ssl_stream->handshake(boost::asio::ssl::stream_base::client, error);
      if (auto check = check_boost_error_and_cancelled(error); check)
        return check;
    }
    const auto prefixed_target = server.target_prefix + target;
    boost::beast::http::request<boost::beast::http::string_body> req{
        verb, prefixed_target, 11};
    req.set(boost::beast::http::field::host, server.host);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (server.secure)
      boost::beast::http::write(*ssl_stream, req, error);
    else
      boost::beast::http::write(stream, req, error);
    if (auto check = check_boost_error_and_cancelled(error); check)
      return check;
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    if (server.secure)
      boost::beast::http::read(*ssl_stream, buffer, res, error);
    else
      boost::beast::http::read(stream, buffer, res, error);
    if (auto check = check_boost_error_and_cancelled(error); check)
      return check;

    auto json_body = nlohmann::json::parse(res.body(), nullptr, false);
    if (json_body.is_discarded())
      return http2::error_code::could_not_parse_json;
    auto response =
        http2::response{static_cast<int>(res.result()),
                        std::make_unique<nlohmann::json>(std::move(json_body))};
    return response;
  }

  void cancel() {
    BOOST_ASSERT(!cancelled);
    cancelled = true;
  }

private:
  const std::shared_ptr<boost::asio::io_context> io_context;
  const http2::server server;
  const boost::beast::http::verb verb;
  const std::string target;
  bool cancelled{};
  std::thread run_thread;
};

class request : public http2::request {
public:
  request(std::shared_ptr<boost::asio::io_context> io_context,
          http2::server server, boost::beast::http::verb verb,
          std::string target)
      : delegate{std::make_shared<request_thread_safe>(io_context, server, verb,
                                                       target)} {}

  void
  async_run(std::function<void(http2::response_result)> callback) override {
    delegate->async_run([delegate = delegate,
                         lifetime_check = std::weak_ptr<int>{lifetime},
                         callback](auto result) {
      if (!lifetime_check.lock())
        return;
      callback(std::move(result));
    });
  }

  http2::response_result run() override { return delegate->run(); };

private:
  std::shared_ptr<request_thread_safe> delegate;
  std::shared_ptr<int> lifetime = std::make_shared<int>();
};

class requester : public http2::requester {
public:
  requester(std::shared_ptr<boost::asio::io_context> io_context,
            http2::server server)
      : io_context{io_context}, server{server} {}

  std::unique_ptr<http2::request> get(std::string target) override {
    return std::make_unique<request>(io_context, server,
                                     boost::beast::http::verb::get, target);
  }

  std::unique_ptr<http2::request> post(std::string target,
                                       nlohmann::json body) override {
    BOOST_ASSERT(false);
    return nullptr;
  }
  std::unique_ptr<http2::request> put(std::string target,
                                      nlohmann::json body) override {
    BOOST_ASSERT(false);
    return nullptr;
  }

private:
  const std::shared_ptr<boost::asio::io_context> io_context;
  const http2::server server;
};

class FUBBLE_PUBLIC factory : public http2::factory {
public:
  factory(std::shared_ptr<utils::executor_module> executor)
      : executor{executor} {}

  std::unique_ptr<http2::requester>
  create_requester(http2::server server_) override {
    return std::make_unique<requester>(executor->get_io_context(), server_);
  }

  std::unique_ptr<http2::requester>
  create_websocket_client(http2::server server_) override {
    BOOST_ASSERT(false);
    return nullptr;
  }

private:
  std::shared_ptr<utils::executor_module> executor;
};

} // namespace

std::unique_ptr<http2::factory> fubble::http_beast::create_factory(
    std::shared_ptr<utils::executor_module> executor) {
  return std::make_unique<factory>(executor);
}
