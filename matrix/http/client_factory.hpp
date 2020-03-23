#ifndef UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A
#define UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A

#include "client.hpp"
#include <memory>

namespace matrix::http {
class client_factory {
public:
  client_factory(boost::asio::io_context &context, client::server server,
                 client::fields fields_)
      : context(context), server{server}, fields_{fields_} {}

  inline std::unique_ptr<client> create() {
    return std::make_unique<client>(context, server, fields_);
  }

  inline std::unique_ptr<client> create(const client::fields &fields) {
    return std::make_unique<client>(context, server, fields);
  }

  inline client::fields get_fields() const { return fields_; }

private:
  boost::asio::io_context &context;
  client::server server;
  client::fields fields_;
};
} // namespace matrix::http

#endif
