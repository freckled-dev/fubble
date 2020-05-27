#ifndef UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7
#define UUID_FA86766A_7231_4254_9E64_C74DEBFFBAC7

#include "http/logger.hpp"
#include "server.hpp"
#include <boost/thread/future.hpp>

namespace http {
class https_connection;
class ssl_upgrader {
public:
  ssl_upgrader(const server &server_, https_connection &connection_);
  ~ssl_upgrader();

  boost::future<void> secure_connection();

protected:
  void on_secured(const boost::system::error_code &error);
  bool check_and_handle_error(const boost::system::error_code &error);

  http::logger logger{"ssl_upgrader"};
  const server server_;
  https_connection &connection_;
  std::shared_ptr<boost::promise<void>> promise =
      std::make_shared<boost::promise<void>>();
};
} // namespace http

#endif
