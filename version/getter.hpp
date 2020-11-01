#ifndef UUID_ED5E157A_5047_42B6_8B24_4BF02EE34F3B
#define UUID_ED5E157A_5047_42B6_8B24_4BF02EE34F3B

#include "http/client.hpp"
#include <boost/thread/future.hpp>

namespace version {
class getter {
public:
  virtual ~getter() = default;

  struct result {
    std::string minimum_version;
    std::string current_version;
  };
  virtual boost::future<result> get() = 0;

  static std::unique_ptr<getter>
  create(std::shared_ptr<http::client> http_client);
};
} // namespace version

#endif
