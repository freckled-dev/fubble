#include "di_websocket.hpp"
#include "boost_di_extension_scopes_session.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_creator.hpp"

boost::di::injector<websocket::acceptor &>
di_websocket::make(boost::asio::io_context &context) {
  namespace di = boost::di;
  return di::make_injector<di::extension::shared_config>(
      di::bind<boost::asio::io_context>.to(context));
}
