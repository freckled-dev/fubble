#ifndef UUID_9A234102_E6DB_48C0_A9DB_0B4DA3C84747
#define UUID_9A234102_E6DB_48C0_A9DB_0B4DA3C84747

#include "connection.hpp"
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/variant.hpp>

namespace http {

using http_type = boost::beast::tcp_stream;
using ssl_type = boost::beast::ssl_stream<http_type &>;
using http_or_https = boost::variant<http_type *, ssl_type *>;

} // namespace http

#endif
