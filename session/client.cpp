#include "client.hpp"
#include "uuid.hpp"
#include <fmt/format.h>

using namespace session;

client::client(std::unique_ptr<matrix::client> matrix_client)
    : matrix_client(std::move(matrix_client)) {}

client::~client() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "~client()";
}

void client::close() {
  // TODO
}

const matrix::client &client::get_natives() const { return *matrix_client; }

matrix::client &client::get_natives() { return *matrix_client; }

boost::future<void> client::set_name(const std::string &name) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "setting name to:" << name;
  return boost::make_ready_future();
}
