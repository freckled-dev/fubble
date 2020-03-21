#ifndef UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A
#define UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A

#include "client.hpp"
#include <memory>

namespace matrix::http {
class client_factory {
public:
  client_factory(boost::asio::io_context &context, client::server server,
                 client::default_fields fields)
      : context(context), server{server}, fields{fields} {}
  std::unique_ptr<client> create() {
    return std::make_unique<client>(context, server, fields);
  }

private:
  boost::asio::io_context &context;
  client::server server;
  client::default_fields fields;
};
} // namespace matrix::http

#endif
