#ifndef UUID_5800120C_7BB1_4257_A829_71A589906F28
#define UUID_5800120C_7BB1_4257_A829_71A589906F28

#include "connection.hpp"
#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace http {

class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);

  boost::future<std::unique_ptr<connection>> create(const server &server_);

protected:
  boost::asio::io_context &context;
  boost::inline_executor executor;
};
} // namespace http

#endif
