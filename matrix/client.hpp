#ifndef UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B
#define UUID_9B76ABAB_BFE7_44BC_91BF_785B58B45A9B

#include "http/client_factory.hpp"
#include "room_factory.hpp"
#include <boost/thread/executors/inline_executor.hpp>

namespace matrix {
class client {
public:
  struct information {
    std::string user_id;
    std::string access_token;
    std::string device_id;
  };
  client(room_factory &room_factory_, http::client_factory &http_factory,
         const information &information_);

  std::unique_ptr<http::client> create_http_client();

  boost::future<std::unique_ptr<room>> create_room();
  void leave_room(const room &room_);

  void set_display_name();
  void sync();

protected:
  room_factory &room_factory_;
  http::client_factory &http_factory;
  std::unique_ptr<http::client> http_client;
  boost::inline_executor executor;
  const information information_;
};
} // namespace matrix

#endif
