#include "error.hpp"
#include <boost/throw_exception.hpp>
#include <fmt/format.h>

using namespace matrix::error;

response::response(boost::beast::http::status code, const nlohmann::json &error)
    : message(fmt::format("code:'{}', error_json:{}", code, error.dump())) {}

const char *response::what() const noexcept { return message.c_str(); }

void ::matrix::error::check_matrix_response(boost::beast::http::status code,
                                            const nlohmann::json &response_) {
  using http_status = boost::beast::http::status;
  if (code == http_status::ok)
    return;
  BOOST_THROW_EXCEPTION(response(code, response_));
}
