#include "version/application.hpp"
#include <nlohmann/json.hpp>
#include <restinio/all.hpp>

namespace {
class application_impl final : public version::application {
  std::shared_ptr<boost::asio::io_context> context;
  using server_t = restinio::http_server_t<>;
  using settings_t = restinio::server_settings_t<>;
  std::unique_ptr<server_t> server;

public:
  application_impl(std::shared_ptr<boost::asio::io_context> context,
                   const config &config_) {
    auto response_json = nlohmann::json::object();
    response_json["minimum_version"] = config_.minimum_version;
    response_json["current_version"] = config_.current_version;
    auto response_string = response_json.dump();
    server = std::make_unique<server_t>(
        restinio::external_io_context(*context),
        settings_t{}
            .port(config_.port)
            .address(config_.address)
            .request_handler([response_string](auto request) {
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

  ~application_impl() { server->close_sync(); }
};
} // namespace

std::unique_ptr<version::application> version::application::create(
    const std::shared_ptr<boost::asio::io_context> &context,
    const config &config_) {
  return std::make_unique<application_impl>(context, config_);
}
