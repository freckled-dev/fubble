#ifndef UUID_5440DA79_67D3_40F4_860E_6A7BFC8EAC05
#define UUID_5440DA79_67D3_40F4_860E_6A7BFC8EAC05

#include "fields.hpp"
#include "server.hpp"
#include <boost/beast/http/verb.hpp>
#include <memory>

namespace http {
class action;
class connection_creator;
class action_factory {
public:
  action_factory(connection_creator &connection_creator_);

  std::unique_ptr<action> create(const server &server_,
                                 const std::string &target,
                                 const fields &fields_,
                                 boost::beast::http::verb verb);

protected:
  connection_creator &connection_creator_;
};
} // namespace http

#endif
