#include "client.hpp"
#include "matrix/client.hpp"
#include "room.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

using namespace session;

client::client(std::unique_ptr<matrix::client> matrix_client)
    : matrix_client(std::move(matrix_client)) {}

client::~client() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~client()";
}

boost::future<void> client::run() { return matrix_client->sync_till_stop(); }

void client::close() { matrix_client->stop_sync(); }

boost::future<void> client::leave_room(room &room_) {
  auto room_id = room_.get_id();
  return matrix_client->get_rooms().leave_room_by_id(room_id);
}

std::string client::get_id() const { return matrix_client->get_user_id(); }

const matrix::client &client::get_natives() const { return *matrix_client; }

matrix::client &client::get_natives() { return *matrix_client; }

boost::future<void> client::set_name(const std::string &name) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "setting name to:" << name;
  return matrix_client->set_display_name(name);
}
