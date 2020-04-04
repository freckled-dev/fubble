#ifndef UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A
#define UUID_0DCA8F3B_4931_4A9F_AAD9_32BE9B13C71A

#include "client.hpp"
#include <memory>
#include <utility>

namespace http {
class client_factory {
public:
  client_factory(boost::asio::io_context &context,
                 const std::pair<server, fields> &fields_)
      : client_factory(context, fields_.first, fields_.second) {}

  client_factory(boost::asio::io_context &context, server server_,
                 fields fields_)
      : context(context), server_{server_}, fields_{fields_} {}

  inline std::unique_ptr<client> create() {
    return std::make_unique<client>(context, server_, fields_);
  }

  inline std::unique_ptr<client> create(const fields &fields) {
    return std::make_unique<client>(context, server_, fields);
  }

  inline fields get_fields() const { return fields_; }

private:
  boost::asio::io_context &context;
  server server_;
  fields fields_;
};
} // namespace http

#endif
