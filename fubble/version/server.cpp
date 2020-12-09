#include "version/server.hpp"
#include "version/logger.hpp"
#include <nlohmann/json.hpp>
#include <restinio/all.hpp>

namespace {
class server_impl final : public version::server {
  version::logger logger{"server_impl"};
  std::shared_ptr<boost::asio::io_context> context_shared;
  boost::asio::io_context &context;
  const config config_;
  using server_t = restinio::http_server_t<>;
  using settings_t = restinio::server_settings_t<>;
  std::unique_ptr<server_t> server;

public:
  server_impl(std::shared_ptr<boost::asio::io_context> context,
              const config &config_)
      : context_shared{context}, context(*context.get()), config_{config_} {
    start();
  }

  server_impl(boost::asio::io_context &context, const config &config_)
      : context(context), config_{config_} {
    start();
  }

  void start() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    auto response_json = nlohmann::json::object();
    response_json["minimum_version"] = config_.minimum_version;
    response_json["current_version"] = config_.current_version;
    auto response_string = response_json.dump();
    server = std::make_unique<server_t>(
        restinio::external_io_context(context),
        settings_t{}
            .port(config_.port)
            .address(config_.address)
            .acceptor_post_bind_hook([this](auto &acceptor) {
              port = acceptor.local_endpoint().port();
              BOOST_LOG_SEV(logger, logging::severity::debug)
                  << "port got set to:" << port;
            })
            .request_handler([response_string, this](auto request) {
              BOOST_LOG_SEV(logger, logging::severity::debug)
                  << "request:" << *request << ", response:" << response_string;
              // https://stiffstream.com/en/docs/restinio/0.6/responsebuilder.html
              auto response = request->create_response();
#if 0
              response.append_header(restinio::http_field::server, "RESTinio server");
#endif
              response.append_header_date_field();
              response.append_header(restinio::http_field::content_type,
                                     "application/json");
              response.set_body(response_string);
              return response.done();
            }));
    server->open_sync();
  }

  ~server_impl() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    server->close_sync();
  }

  int get_port() const override {
    BOOST_ASSERT(port != 0);
    return port;
  }

protected:
  int port{};
};
} // namespace

std::unique_ptr<version::server>
version::server::create(const std::shared_ptr<boost::asio::io_context> &context,
                        const config &config_) {
  return std::make_unique<server_impl>(context, config_);
}

std::unique_ptr<version::server> version::server::create(
    const std::shared_ptr<boost::asio::io_context> &context) {
  const config config_;
  return std::make_unique<server_impl>(context, config_);
}

std::unique_ptr<version::server>
version::server::create(boost::asio::io_context &context) {
  const config config_;
  return std::make_unique<server_impl>(context, config_);
}
