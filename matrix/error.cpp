#include "error.hpp"
#include <fmt/format.h>

using namespace matrix::error;

response::response(boost::beast::http::status code, const nlohmann::json &error)
    : std::runtime_error(
          fmt::format("code:'{}', error_json:{}", code, error.dump())) {}
