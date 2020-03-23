#include "client_factory.hpp"

using namespace matrix;

client_factory::client_factory(room_factory &room_factory_,
                               http::client_factory &http_client_factory)
    : room_factory_(room_factory_), http_client_factory(http_client_factory) {}

std::unique_ptr<client>
client_factory::create(const client::information &information) {
  return std::make_unique<client>(room_factory_, http_client_factory,
                                  information);
}
