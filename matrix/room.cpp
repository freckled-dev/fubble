#include "room.hpp"
#include "client.hpp"

using namespace matrix;

room::room(client &client_, const std::string &id) : id(id) { (void)client_; }
