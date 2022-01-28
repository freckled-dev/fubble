#include "client_module.hpp"
#include <boost/asio/dispatch.hpp>
#include <system_error>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <thread>

// TODO https://www.boost.org/doc/libs/1_78_0/libs/outcome/doc/html/motivation/plug_error_code.html
enum class http2_error_code { success, could_not_parse_json, cancelled };
struct error_category : std::error_category {
  const char *name() const noexcept override { return "http errors"; }
  std::string message(int ev) const override {

    switch (static_cast<http2_error_code>(ev)) {
    case http2_error_code::success:
      return "success";
    case http2_error_code::could_not_parse_json:
      return "could not parse json";
    case http2_error_code::cancelled:
      return "destroyed before done";
    }
  }
};
error_category error_category_instance;
namespace std
{
  template <> struct is_error_code_enum<http2_error_code> : true_type { };
}

using namespace fubble::http_beast;
using namespace fubble;

namespace {

std::error_code make_http_error(http2_error_code code) {
  return std::error_code{static_cast<int>(code), error_category_instance};
}

using tcp = boost::asio::ip::tcp;

class request_thread_safe : std::enable_shared_from_this<request_thread_safe>,
                            http2::request {
public:
  request_thread_safe(std::shared_ptr<boost::asio::io_context> io_context,
                      http2::server server, boost::beast::http::verb verb,
                      std::string target)
      : io_context{io_context}, server{server}, verb{verb}, target{target} {}

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
      return make_http_error(http2_error_code::cancelled);
    stream.connect(results, error);
    if (error)
      return error;
    if (cancelled)
      return make_http_error(http2_error_code::cancelled);
    boost::beast::http::request<boost::beast::http::string_body> req{
        verb, target, 11};
    req.set(boost::beast::http::field::host, local_server.host);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    boost::beast::http::write(stream, req, error);
    if (error)
      return error;
    if (cancelled)
      return make_http_error(http2_error_code::cancelled);
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream, buffer, res, error);
    if (error)
      return error;
    if (cancelled)
      return make_http_error(http2_error_code::cancelled);

    auto json_body = nlohmann::json::parse(res.body(), nullptr, false);
    if (json_body.is_discarded())
      return make_http_error(http2_error_code::could_not_parse_json);
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
