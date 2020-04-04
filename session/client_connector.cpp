#include "client_connector.hpp"
#include "client.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

using namespace session;

client_connector::client_connector(
    client_factory &client_factory_,
    matrix::authentification &matrix_authentification)
    : client_factory_(client_factory_),
      matrix_authentification(matrix_authentification) {}

boost::future<client_connector::client_ptr> client_connector::connect() {
  return matrix_authentification.register_as_guest().then(
      [this](auto result) { return client_factory_.create(result.get()); });
}

