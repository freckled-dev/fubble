#ifndef UUID_BEFED083_8CFB_495F_87DB_332DCD81E725
#define UUID_BEFED083_8CFB_495F_87DB_332DCD81E725

#include <boost/beast/http/status.hpp>
#include <boost/exception/exception.hpp>
#include <nlohmann/json.hpp>

namespace matrix::error {

// derive of boost::exception is neccessary, so it's boost::future compatible
class response : public virtual boost::exception,
                 public virtual std::exception {
  std::string message;

public:
  response(boost::beast::http::status code, const nlohmann::json &error);

  const char *what() const noexcept override;
};

void check_matrix_response(boost::beast::http::status code,
                           const nlohmann::json &response_);
} // namespace matrix::error

#endif
