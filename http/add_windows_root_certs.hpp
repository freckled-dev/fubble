#ifndef FUBBLE_HTTP_ADD_WINDOWS_ROOT_CERTS_HPP
#define FUBBLE_HTTP_ADD_WINDOWS_ROOT_CERTS_HPP

#include <boost/asio/ssl/context.hpp>

namespace http {
// https://stackoverflow.com/questions/39772878/reliable-way-to-get-root-ca-certificates-on-windows
void add_windows_root_certs(boost::asio::ssl::context &context);
}

#endif