#include "connection.hpp"

websocket::connection::connection(boost::asio::io_context &context)
    : socket{context} {}

websocket::connection::~connection() = default;
