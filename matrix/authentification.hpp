#ifndef UUID_1364323B_4F34_48AA_A740_45367F40E78D
#define UUID_1364323B_4F34_48AA_A740_45367F40E78D

#include "http/client.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class authentification {
  boost::inline_executor executor;
  http::client &http_client;

public:
  authentification(http::client &http_client);
  struct login_result {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  boost::future<login_result> register_as_guest();
  boost::future<login_result> register_(const std::string &username,
                                        const std::string &password);
  boost::future<login_result> login(const std::string &username,
                                    const std::string &password);
};
} // namespace matrix

#endif
