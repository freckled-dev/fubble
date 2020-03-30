#include "client_connector.hpp"
#include "client.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

using namespace session;
using namespace session;

client_connector::client_connector(client &client_) : client_(client_) {}

boost::future<void> client_connector::connect() {
  // TODO authentification
  return boost::make_ready_future();
}

