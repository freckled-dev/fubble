#ifndef UUID_5800120C_7BB1_4257_A829_71A589906F28
#define UUID_5800120C_7BB1_4257_A829_71A589906F28

#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace http {

// TODO refactor to factory. unite all factories of the http module to one
class connector;
class connection;
class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);

  std::unique_ptr<connection> create(const server &server_);
  std::unique_ptr<connector> create_connector(connection &connection_,
                                              const server &server_);

protected:
  boost::asio::io_context &context;
  boost::inline_executor executor;
};
} // namespace http

#endif
