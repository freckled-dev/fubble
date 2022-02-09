#include "client_factory.hpp"

using namespace matrix;

client_factory::client_factory(
    factory &room_factory_,
    const std::shared_ptr<fubble::http2::factory> &http_client_factory)
    : room_factory_(room_factory_), http_client_factory(http_client_factory) {}

std::unique_ptr<client>
client_factory::create(const client::information &information) {
  // TODO http_client_factory->create_requester();
  std::shared_ptr<fubble::http2::requester> requester;
  return std::make_unique<client>(room_factory_, requester, information);
}
