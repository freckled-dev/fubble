#include "module.hpp"
#include <boost/assert.hpp>
#include <nlohmann/json.hpp>

struct http2_error_category : std::error_category {
  const char *name() const noexcept override { return "http error"; }
  std::string message(int ev) const override {
    switch (static_cast<fubble::http2::error_code>(ev)) {
    case fubble::http2::error_code::success:
      return "success";
    case fubble::http2::error_code::could_not_parse_json:
      return "could not parse json";
    case fubble::http2::error_code::cancelled:
      return "destroyed before done";
    default:
      BOOST_ASSERT(false);
      return "unknown error";
    }
  }
};

FUBBLE_PUBLIC const http2_error_category &get_http2_error_category() {
  static http2_error_category result;
  return result;
}

std::error_code fubble::http2::make_error_code(error_code code) {
  return {static_cast<int>(code), get_http2_error_category()};
}

fubble::http2::response::~response() = default;
