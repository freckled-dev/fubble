#include "connection.hpp"

using namespace websocket;

connection::connection(boost::asio::io_context &context) : stream(context) {}

connection::stream_type &connection::get_native() { return stream; }
