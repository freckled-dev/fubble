#include "error.hpp"
#include <boost/throw_exception.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

using namespace matrix::error;

response::response(boost::beast::http::status code, const nlohmann::json &error)
    : message(fmt::format("code:'{}', error_json:{}", code, error.dump())) {}

const char *response::what() const noexcept { return message.c_str(); }

void ::matrix::error::check_matrix_response(
    boost::future<std::pair<boost::beast::http::status, nlohmann::json>>
        &response_) {
  auto got = response_.get();
  check_matrix_response(got);
}

void ::matrix::error::check_matrix_response(
    std::pair<boost::beast::http::status, nlohmann::json> &response_) {
  check_matrix_response(response_.first, response_.second);
}

void ::matrix::error::check_matrix_response(boost::beast::http::status code,
                                            const nlohmann::json &response_) {
  using http_status = boost::beast::http::status;
  if (code == http_status::ok)
    return;
  BOOST_THROW_EXCEPTION(response(code, response_));
}
