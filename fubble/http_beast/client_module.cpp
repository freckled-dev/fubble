#include "client_module.hpp"
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <thread>

using namespace fubble::http_beast;
using namespace fubble;

namespace {

enum class error_code { could_not_parse_json, cancelled };
struct error_category : std::error_category {
  const char *name() const noexcept override { return "http errors"; }
  std::string message(int ev) const override {

    switch (static_cast<error_code>(ev)) {
    case error_code::could_not_parse_json:
      return "could not parse json";
    case error_code::cancelled:
      return "destroyed before done";
    }
  }
};
error_category error_category_instance;
std::error_code make_error(error_code code) {
  return std::error_code{static_cast<int>(code), error_category_instance};
}

using tcp = boost::asio::ip::tcp;

class request_thread_safe : std::enable_shared_from_this<request_thread_safe>,
                            http2::request {
public:
  request_thread_safe(std::shared_ptr<boost::asio::io_context> io_context,
                      http2::server server)
      : io_context{io_context}, server{server} {}

  // TODO port to real async
  void
  async_run(std::function<void(http2::response_result)> callback_) override {
    BOOST_ASSERT(callback_);
    std::thread thread{[this, callback_, thiz = shared_from_this()]() {
      auto result = run();
      // switch back to main thread
      boost::asio::dispatch(*io_context, [result = std::move(result), callback_,
                                          thiz = shared_from_this()]() mutable {
        callback_(std::move(result));
      });
    }};
  }

  http2::response_result run() override {
    // in the async variant the class may get destructed while `run` is getting
    // called. so keep local copies.
    auto local_server = server;
    tcp::resolver resolver(*io_context);
    boost::beast::tcp_stream stream(*io_context);
    boost::beast::error_code error;
    auto const results =
        resolver.resolve(local_server.host, local_server.service, error);
    if (error)
      return error;
    if (cancelled)
      return make_error(error_code::cancelled);
    stream.connect(results, error);
    if (error)
      return error;
    if (cancelled)
      return make_error(error_code::cancelled);
    boost::beast::http::request<boost::beast::http::string_body> req{
        verb, target, 11};
    req.set(boost::beast::http::field::host, local_server.host);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    boost::beast::http::write(stream, req, error);
    if (error)
      return error;
    if (cancelled)
      return make_error(error_code::cancelled);
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream, buffer, res, error);
    if (error)
      return error;
    if (cancelled)
      return make_error(error_code::cancelled);

    auto json_body = nlohmann::json::parse(res.body(), nullptr, false);
    if (json_body.is_discarded())
      return make_error(error_code::could_not_parse_json);
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
  std::shared_ptr<boost::asio::io_context> io_context;
  http2::server server;
  boost::beast::http::verb verb;
  std::string target;
  bool cancelled{};
};

class request : http2::request {
public:
  request(std::shared_ptr<boost::asio::io_context> io_context,
          http2::server server)
      : delegate{std::make_shared<request_thread_safe>(io_context, server)} {}

  void
  async_run(std::function<void(http2::response_result)> callback) override {
    delegate->async_run(
        [lifetime_check = std::weak_ptr<int>{lifetime}, callback](auto result) {
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
  std::unique_ptr<http2::request> get(std::string target) override {
    BOOST_ASSERT(false);
    return nullptr;
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

  std::shared_ptr<boost::asio::io_context> io_context;
  http2::server server;
};

} // namespace
