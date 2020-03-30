#ifndef UUID_8CFB5B2F_0E59_4213_AE28_1EEF84B422AC
#define UUID_8CFB5B2F_0E59_4213_AE28_1EEF84B422AC

#include <boost/beast/http/status.hpp>
#include <boost/exception/exception.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>
#include <nlohmann/json.hpp>

namespace http {
class client;
}
namespace temporary_room::net {
class error : public virtual boost::exception, public virtual std::exception {
  std::string message;

public:
  error(boost::beast::http::status code, const nlohmann::json &error);
  const char *what() const noexcept override;
};

class client {
public:
  client(http::client &http_client);

  boost::future<std::string> join(const std::string &name, const std::string& user_id);

protected:
  http::client &http_client;
  boost::inline_executor executor;
};
} // namespace temporary_room::net

#endif
