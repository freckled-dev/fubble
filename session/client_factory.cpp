#include "client_factory.hpp"

using namespace session;

std::unique_ptr<client>
client_factory::create(std::unique_ptr<matrix::client> matrix_client) {
  return std::make_unique<client>(std::move(matrix_client));
}

