#ifndef UUID_BEFED083_8CFB_495F_87DB_332DCD81E725
#define UUID_BEFED083_8CFB_495F_87DB_332DCD81E725

#include <boost/beast/http/status.hpp>
#include <nlohmann/json.hpp>

namespace matrix::error {

class response : public std::runtime_error {
public:
  response(boost::beast::http::status code, const nlohmann::json &error);
};

inline void check_matrix_response(boost::beast::http::status code,
                                  const nlohmann::json &response_) {
  using http_status = boost::beast::http::status;
  if (code == http_status::ok)
    return;
  throw response(code, response_);
}
} // namespace matrix::error

#endif
